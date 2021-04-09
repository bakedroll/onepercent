#include "MainMenuState.h"
#include "GlobeInteractionState.h"

#include <osgHelper/View.h>

#include <QtOsgBridge/Helper.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>

namespace onep
{

  struct MainMenuState::Impl
  {
    Impl()
    {}

    QPointer<QWidget> overlay;
  };

  MainMenuState::MainMenuState(osgHelper::ioc::Injector& injector)
	  : GlobeCameraState(injector)
    , m(new Impl())
  {
  }

  MainMenuState::~MainMenuState() = default;

  void MainMenuState::onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data)
  {
    GlobeCameraState::onInitialize(mainWindow, data);

	  setCameraDistance(9.5f);
	  setCameraViewAngle(osg::Vec2f(-0.9f, 0.6f));
	  setCameraLatLong(osg::Vec2f(0.5f, 1.2f));

    setupUi(mainWindow);
  }

  void MainMenuState::onUpdate(const SimulationData& data)
  {
	  setCameraLatLong(getCameraLatLong() + osg::Vec2f(0.0f, -data.timeDelta * 0.02f));

    return GlobeCameraState::onUpdate(data);
  }

  void MainMenuState::onExit()
  {
    QtOsgBridge::Helper::deleteWidget(m->overlay);
  }

  void MainMenuState::onResizeEvent(QResizeEvent* event)
  {
    m->overlay->setGeometry(0, 0, event->size().width(), event->size().height());
  }

  bool MainMenuState::onKeyEvent(QKeyEvent* event)
  {
    if ((event->type() == QKeyEvent::Type::KeyPress) && (event->key() == Qt::Key::Key_S))
    {
      requestNewEventState<GlobeInteractionState>(NewEventStateMode::ExitCurrent);
      return true;
    }

    return false;
  }

  void MainMenuState::setupUi(QPointer<QtOsgBridge::MainWindow> mainWindow)
  {
    QPushButton* buttonStart = new QPushButton(QObject::tr("Start Game"));
    QPushButton* buttonEnd = new QPushButton(QObject::tr("End Game"));

    buttonStart->setObjectName("ButtonMainMenu");
    buttonEnd->setObjectName("ButtonMainMenu");

    connect(buttonStart, &QPushButton::clicked, [this]()
    {
      requestNewEventState<GlobeInteractionState>(NewEventStateMode::ExitCurrent);
    });

    connect(buttonEnd, &QPushButton::clicked, [this]()
    {
      requestExitEventState(ExitEventStateMode::ExitAll);
    });

    auto resolution = mainWindow->getViewWidget()->getView()->getResolution();

    m->overlay = new QWidget();
    m->overlay->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

    auto layout = new QVBoxLayout();
    layout->addStretch(5);
    layout->addWidget(buttonStart);
    layout->addWidget(buttonEnd);
    layout->addStretch(7);

    layout->setAlignment(Qt::AlignHCenter);

    m->overlay->setLayout(layout);

    mainWindow->getViewWidget()->addOverlayWidget(m->overlay);
  }

}