#include "MainMenuState.h"
#include "GlobeInteractionState.h"

#include "core/QConnectFunctor.h"
#include "nodes/GlobeOverviewWorld.h"
#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/Hud.h>
#include <osgGaming/View.h>

#include <QVBoxLayout>
#include <QPushButton>

using namespace osg;
using namespace onep;
using namespace osgGaming;

struct MainMenuState::Impl
{
  Impl() 
    : overlay(nullptr)
  {}

  VirtualOverlay* overlay;
};

MainMenuState::MainMenuState(osgGaming::Injector& injector)
	: GlobeCameraState()
  , m(new Impl())
{

}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::initialize()
{
	GlobeCameraState::initialize();

	setCameraDistance(9.5f);
	setCameraViewAngle(Vec2f(-0.9f, 0.6f));
	setCameraLatLong(Vec2f(0.5f, 1.2f));

  getHud(getView(0))->setFpsEnabled(true);

  setupUi();
}

GameState::StateEvent* MainMenuState::update()
{
	setCameraLatLong(getCameraLatLong() + Vec2f(0.0f, -getFrameTime() * 0.02f));

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
    stateEvent_replace(new GlobeInteractionState());
  }
}

ref_ptr<Hud> MainMenuState::overrideHud(osg::ref_ptr<osgGaming::View> view)
{
	return new Hud();
}

void MainMenuState::setupUi()
{
  QPushButton* buttonStart = new QPushButton(QObject::tr("Start Game"));
  QPushButton* buttonEnd = new QPushButton(QObject::tr("End Game"));

  buttonStart->setObjectName("ButtonMainMenu");
  buttonEnd->setObjectName("ButtonMainMenu");

  QConnectFunctor::connect(buttonStart, SIGNAL(clicked()), [this]()
  {
    stateEvent_replace(new GlobeInteractionState());
  });

  QConnectFunctor::connect(buttonEnd, SIGNAL(clicked()), [this]()
  {
    stateEvent_endGame();
  });

  osg::Vec2f resolution = getView(0)->getResolution();

  m->overlay = new VirtualOverlay();
  m->overlay->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addStretch(5);
  layout->addWidget(buttonStart);
  layout->addWidget(buttonEnd);
  layout->addStretch(7);

  layout->setAlignment(Qt::AlignHCenter);

  m->overlay->setLayout(layout);

  getOverlayCompositor()->addVirtualOverlay(m->overlay);
}