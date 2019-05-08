#include "MainMenuState.h"
#include "GlobeInteractionState.h"

#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/Hud.h>
#include <osgGaming/View.h>

#include <QVBoxLayout>
#include <QPushButton>

namespace onep
{

  struct MainMenuState::Impl
  {
    Impl() 
      : overlay(nullptr)
    {}

    VirtualOverlay* overlay;
  };

  MainMenuState::MainMenuState(osgGaming::Injector& injector)
	  : GlobeCameraState(injector)
    , m(new Impl())
  {
  }

  MainMenuState::~MainMenuState() = default;

  void MainMenuState::initialize()
  {
	  GlobeCameraState::initialize();

	  setCameraDistance(9.5f);
	  setCameraViewAngle(osg::Vec2f(-0.9f, 0.6f));
	  setCameraLatLong(osg::Vec2f(0.5f, 1.2f));

    setupUi();
  }

  osgGaming::GameState::StateEvent* MainMenuState::update()
  {
	  setCameraLatLong(getCameraLatLong() + osg::Vec2f(0.0f, -getFrameTime() * 0.02f));

	  return GlobeCameraState::update();
  }

  void MainMenuState::onResizeEvent(float width, float height)
  {
    m->overlay->setGeometry(0, 0, width, height);
  }

  void MainMenuState::onKeyPressedEvent(int key)
  {
    if (key == 's')
    {
      stateEvent_replace<GlobeInteractionState>();
    }
  }

  osg::ref_ptr<osgGaming::Hud> MainMenuState::injectHud(osgGaming::Injector& injector, osg::ref_ptr<osgGaming::View> view)
  {
	  return injector.inject<osgGaming::Hud>();
  }

  void MainMenuState::setupUi()
  {
    QPushButton* buttonStart = new QPushButton(QObject::tr("Start Game"));
    QPushButton* buttonEnd = new QPushButton(QObject::tr("End Game"));

    buttonStart->setObjectName("ButtonMainMenu");
    buttonEnd->setObjectName("ButtonMainMenu");

    QObject::connect(buttonStart, &QPushButton::clicked, [this]()
    {
      stateEvent_replace<GlobeInteractionState>();
    });

    QObject::connect(buttonEnd, &QPushButton::clicked, [this]()
    {
      stateEvent_endGame();
    });

    auto resolution = getView(0)->getResolution();

    m->overlay = new VirtualOverlay();
    m->overlay->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

    auto layout = new QVBoxLayout();
    layout->addStretch(5);
    layout->addWidget(buttonStart);
    layout->addWidget(buttonEnd);
    layout->addStretch(7);

    layout->setAlignment(Qt::AlignHCenter);

    m->overlay->setLayout(layout);

    getOverlayCompositor()->addVirtualOverlay(m->overlay);
  }

}