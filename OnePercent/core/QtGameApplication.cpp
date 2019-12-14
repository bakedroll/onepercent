#include "QtGameApplication.h"

#include "states/QtGameState.h"
#include "states/QtGameLoadingState.h"
#include "widgets/MainWindow.h"
#include "widgets/OverlayCompositor.h"

#include <osgGaming/InputManager.h>
#include <osgGaming/Observable.h>
#include <osgGaming/LogManager.h>

#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QMessageBox>

#include <assert.h>

namespace onep
{

class FileLogger : public osgGaming::Logger
{
public:
  FileLogger(const QString& directory)
  {
    QDir dir(directory);
    if (!dir.exists())
      dir.mkpath(".");

    QString filename = dir.filePath(QString("%1.txt").arg(QDateTime::currentDateTime().toString("yy-MM-dd_HH.mm.ss")));

    m_file.setFileName(filename);
    m_file.open(QIODevice::WriteOnly);

    m_textStream.setDevice(&m_file);
  }

  ~FileLogger()
  {
    if (m_file.isOpen())
      m_file.close();
  }

  virtual void log(const std::string& text) override
  {
    if (m_file.isOpen())
    {
      m_textStream << QString::fromStdString(text) << "\n";
      m_textStream.flush();
    }
  }

private:
  QFile m_file;
  QTextStream m_textStream;
};

struct QtGameApplication::Impl
{
  Impl()
    : mainWindow(nullptr)
    , overlayCompositor(nullptr)
    , oFullscreenEnabled(new osgGaming::Observable<bool>(false))
  {
  }

  MainWindow* mainWindow;
  std::shared_ptr<OverlayCompositor> overlayCompositor;

  osgGaming::Observable<bool>::Ptr oFullscreenEnabled;

  osgGaming::Observer<void>::Ptr endGameObserver;
  osgGaming::Observer<bool>::Ptr fullscreenEnabledObserver;
};

QtGameApplication::QtGameApplication(int& argc, char** argv)
  : Multithreading(argc, argv)
  , GameApplication()
  , m(new Impl())
{
  osgGaming::LogManager::getInstance()->addLogger(new osgGaming::StdOutLogger());
  osgGaming::LogManager::getInstance()->addLogger(new FileLogger("./Logs"));

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

QtGameApplication::~QtGameApplication() = default;

bool QtGameApplication::notify(QObject* receiver, QEvent* event)
{
  if (safeExecute([&](){ Multithreading::notify(receiver, event); return 0; }))
    return true;

  return false;
}

void QtGameApplication::deinitialize()
{
  OSGG_LOG_INFO("Application shutting down");

  m->overlayCompositor.reset();

  m->mainWindow->close();
  delete m->mainWindow;
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
  osg::ref_ptr<osgGaming::View> view = getViewer()->getGamingView(0);
  osg::ref_ptr<osgGaming::Hud> hud = state->getHud(view);

  if (m->overlayCompositor->getContainer()->getNumParents() > 0)
    m->overlayCompositor->getContainer()->getParent(0)->removeChild(m->overlayCompositor->getContainer());

  if (!hud.valid())
    return;

  hud->getModelViewTransform()->addChild(m->overlayCompositor->getContainer());
}

int QtGameApplication::mainloop()
{
  getViewer()->realize();

  return exec();
}

osg::ref_ptr<osgGaming::InputManager> QtGameApplication::createInputManager(osg::ref_ptr<osgGaming::View> view)
{
  assert(m->mainWindow->getViewWidget());
  return m->mainWindow->getViewWidget();
}

void QtGameApplication::onException(const std::string& message)
{
  QMessageBox::critical(nullptr, tr("Fatal error"), tr("A critical exception occured: %1").arg(QString::fromLocal8Bit(message.c_str())));
  quit();
}

}
