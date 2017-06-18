#include "QtGameApplication.h"

#include "nodes/GlobeOverviewWorld.h"
#include "states/QtGameState.h"
#include "states/QtGameLoadingState.h"
#include "widgets/MainWindow.h"
#include "widgets/VirtualOverlay.h"

#include <QGridLayout>
#include <QLayoutItem>

#include <osgGaming/Helper.h>
#include <osgGaming/InputManager.h>
#include <osgGaming/Observable.h>

#include <assert.h>

namespace onep
{

struct QtGameApplication::Impl
{
  Impl()
    : mainWindow(nullptr)
  {
  }

  std::shared_ptr<QApplication> qapplication;

  MainWindow* mainWindow;
  osgGaming::Observer<void>::Ptr endGameObserver;

  osg::ref_ptr<osg::Geode> overlayGeode;

  void ensureOverlayGeode(osg::ref_ptr<osgGaming::Hud> hud, int width, int height)
  {
    if (!overlayGeode.valid())
    {
      overlayGeode = new osg::Geode();
      mainWindow->getViewWidget()->setVirtualOverlayGeode(overlayGeode);
    }
    else
    {
      overlayGeode->removeDrawable(overlayGeode->getDrawable(0));
    }

    osg::ref_ptr<osg::Geometry> geometry = osgGaming::createQuadGeometry(0.0f, float(width) - 1.0f, 0.0f, float(height) - 1.0f, 0.0f, osgGaming::QuadOrientation::XY, false);
    overlayGeode->addDrawable(geometry);

    hud->getModelViewTransform()->addChild(overlayGeode);
  }

  void deleteLayout(QLayout* layout)
  {
    QLayoutItem* item;
    QLayout* sublayout;
    QWidget* widget;
    while ((item = layout->takeAt(0)))
    {
      if ((sublayout = item->layout()) != nullptr)
      {
        deleteLayout(sublayout);
      }
      else if
      ((widget = item->widget()) != nullptr)
      {
        widget->hide();
        delete widget;
      }
      else
      {
        delete item;
      }
    }

    delete layout;
  }
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

  m->endGameObserver = onEndGameSignal().connect(osgGaming::Func<void>([this]()
  {
    m->mainWindow->shutdown();
  }));
}

QtGameApplication::~QtGameApplication()
{
}

void QtGameApplication::stateAttachedEvent(osg::ref_ptr<osgGaming::AbstractGameState> state)
{
  // delete previous overlays
  QLayout* layout = m->mainWindow->getViewWidget()->layout();
  if (layout)
    m->deleteLayout(layout);

  m->mainWindow->getViewWidget()->setVirtualOverlay(nullptr);

  osg::ref_ptr<osgGaming::View> view = getViewer()->getView(0);
  osg::ref_ptr<osgGaming::Hud> hud = state->getHud(view);

  if (!hud.valid())
    return;

  for (int i=0; i<int(hud->getModelViewTransform()->getNumChildren()); i++)
  {
    if (hud->getModelViewTransform()->getChild(i) == m->overlayGeode.get())
    {
      hud->getModelViewTransform()->removeChild(m->overlayGeode);
      break;
    }
  }

  VirtualOverlay* overlay = nullptr;
  QtGameState* qtState = dynamic_cast<QtGameState*>(state.get());
  if (qtState)
  {
    overlay = qtState->createVirtualOverlay();
  }
  else
  {
    QtGameLoadingState* qtLoadingState = dynamic_cast<QtGameLoadingState*>(state.get());
    overlay = qtLoadingState->createVirtualOverlay();
  }

  if (!overlay)
    return;

  QGridLayout* viewLayout = new QGridLayout();
  viewLayout->setContentsMargins(0, 0, 0, 0);
  viewLayout->setSpacing(0);

  viewLayout->addWidget(overlay, 0 , 0);

  m->mainWindow->getViewWidget()->setLayout(viewLayout);
  m->mainWindow->getViewWidget()->setVirtualOverlay(overlay);

  //prepare Hud
  assert(hud.valid());

  int width = int(view->getResolution().x());
  int height = int(view->getResolution().y());

  m->ensureOverlayGeode(hud, width, height);

  overlay->getTexture()->setTextureSize(width, height);
  m->overlayGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, overlay->getTexture(), osg::StateAttribute::ON);

  // first frame
  overlay->renderToTexture();
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
