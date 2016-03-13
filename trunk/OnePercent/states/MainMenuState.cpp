#include "MainMenuState.h"
#include "GlobeInteractionState.h"

#include "nodes/GlobeOverviewWorld.h"

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

	getHud()->loadMarkupFromXmlResource("./GameData/data/ui/mainmenu.xml");

	getHud()->setFpsEnabled(true);
}

GameState::StateEvent* MainMenuState::update()
{
	setCameraLatLong(getCameraLatLong() + Vec2f(0.0f, -getFrameTime() * 0.02f));

	return GlobeCameraState::update();
}

void MainMenuState::onUIClickedEvent(ref_ptr<UIElement> uiElement)
{
	if (uiElement->getUIName() == "button_endGame")
	{
		stateEvent_endGame();
	}
	else if (uiElement->getUIName() == "button_startGame")
	{
		stateEvent_replace(new GlobeInteractionState());
	}
}

ref_ptr<Hud> MainMenuState::newHud()
{
	return new Hud();
}