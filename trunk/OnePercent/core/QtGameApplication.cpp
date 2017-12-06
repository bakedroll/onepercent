#include "QtGameApplication.h"

#include "nodes/GlobeOverviewWorld.h"
#include "states/QtGameState.h"
#include "states/QtGameLoadingState.h"
#include "widgets/MainWindow.h"
#include "widgets/OverlayCompositor.h"

#include <osgGaming/InputManager.h>
#include <osgGaming/Observable.h>

#include <assert.h>

namespace onep
{

struct QtGameApplication::Impl
{
  Impl()
    : mainWindow(nullptr)
    , overlayCompositor(nullptr)
    , oFullscreenEnabled(new osgGaming::Observable<bool>(false))
  {
  }

  std::shared_ptr<QApplication> qapplication;

  MainWindow* mainWindow;
  std::shared_ptr<OverlayCompositor> overlayCompositor;

  osgGaming::Observable<bool>::Ptr oFullscreenEnabled;

  osgGaming::Observer<void>::Ptr endGameObserver;
  osgGaming::Observer<bool>::Ptr fullscreenEnabledObserver;
};

QtGameApplication::QtGameApplication(int argc, char** argv)
  : GameApplication()
  , m(new Impl())
{
  m->qapplication.reset(new QApplication(argc, argv));

  setlocale(LC_NUMERIC, "en_US");

  getViewer()->addView(new osgGaming::View());

  m->mainWindow = new MainWindow(getViewer());

  m->mainWindow->show();

  m->overlayCompositor.reset(new OverlayCompositor());
  m->overlayCompositor->setRootWidget(m->mainWindow->getViewWidget());
  m->mainWindow->getViewWidget()->setOverlayCompositor(m->overlayCompositor);

  m->endGameObserver = onEndGameSignal().connect(osgGaming::Func<void>([this]()
  {
    m->mainWindow->shutdown();
  }));

  m->fullscreenEnabledObserver = m->oFullscreenEnabled->connect(osgGaming::Func<bool>([this](bool enabled)
  {
    if (m->mainWindow->isFullScreen() == enabled)
      return;

    if (enabled)
      m->mainWindow->showFullScreen();
    else
      m->mainWindow->showNormal();
  }));
}

QtGameApplication::~QtGameApplication()
{
}

void QtGameApplication::newStateEvent(osg::ref_ptr<osgGaming::AbstractGameState> state)
{
  m->overlayCompositor->clear();

  QtGameState* qtState = dynamic_cast<QtGameState*>(state.get());
  if (qtState)
  {
    qtState->setOverlayCompositor(m->overlayCompositor);
    qtState->setFullscreenEnabledObs(m->oFullscreenEnabled);
  }
  else
  {
    QtGameLoadingState* qtLoadingState = dynamic_cast<QtGameLoadingState*>(state.get());
    qtLoadingState->setOverlayCompositor(m->overlayCompositor);
    qtLoadingState->setFullscreenEnabledObs(m->oFullscreenEnabled);
  }
}

void QtGameApplication::stateAttachedEvent(osg::ref_ptr<osgGaming::AbstractGameState> state)
{
  osg::ref_ptr<osgGaming::View> view = getViewer()->getView(0);
  osg::ref_ptr<osgGaming::Hud> hud = state->getHud(view);

  if (!hud.valid())
    return;

  if (m->overlayCompositor->getContainer()->getNumParents() > 0)
    m->overlayCompositor->getContainer()->getParent(0)->removeChild(m->overlayCompositor->getContainer());

  hud->getModelViewTransform()->addChild(m->overlayCompositor->getContainer());
}

int QtGameApplication::mainloop()
{
  getViewer()->realize();

  return m->qapplication->exec();
}

osg::ref_ptr<osgGaming::InputManager> QtGameApplication::createInputManager(osg::ref_ptr<osgGaming::View> view)
{
  assert(m->mainWindow->getViewWidget());
  return m->mainWindow->getViewWidget();
}

std::shared_ptr<QApplication> QtGameApplication::qApplication()
{
  return m->qapplication;
}

}
