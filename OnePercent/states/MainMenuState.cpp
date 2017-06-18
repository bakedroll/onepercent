#include "MainMenuState.h"
#include "GlobeInteractionState.h"

#include "core/QConnectFunctor.h"
#include "nodes/GlobeOverviewWorld.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/Hud.h>
#include <osgGaming/View.h>

#include <QVBoxLayout>
#include <QPushButton>

using namespace osg;
using namespace onep;
using namespace osgGaming;

MainMenuState::MainMenuState()
	: GlobeCameraState()
{

}

void MainMenuState::initialize()
{
	GlobeCameraState::initialize();

	setCameraDistance(9.5f);
	setCameraViewAngle(Vec2f(-0.9f, 0.6f));
	setCameraLatLong(Vec2f(0.5f, 1.2f));

  getHud(getView(0))->setFpsEnabled(true);
}

VirtualOverlay* MainMenuState::createVirtualOverlay()
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

  VirtualOverlay* overlay = new VirtualOverlay();

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addStretch(5);
  layout->addWidget(buttonStart);
  layout->addWidget(buttonEnd);
  layout->addStretch(7);

  layout->setAlignment(Qt::AlignHCenter);

  overlay->setLayout(layout);

  return overlay;
}

GameState::StateEvent* MainMenuState::update()
{
	setCameraLatLong(getCameraLatLong() + Vec2f(0.0f, -getFrameTime() * 0.02f));

	return GlobeCameraState::update();
}

ref_ptr<Hud> MainMenuState::overrideHud(osg::ref_ptr<osgGaming::View> view)
{
	return new Hud();
}
