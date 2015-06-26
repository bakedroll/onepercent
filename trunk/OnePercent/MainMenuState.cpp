#include "MainMenuState.h"
#include "GlobeOverviewWorld.h"
#include "GlobeInteractionState.h"

#include <osgGaming/Helper.h>
#include <osgGaming/UIButton.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIText.h>

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

	ref_ptr<UIElement> root = getHud()->getRootUIElement();

	ref_ptr<UIGrid> grid = new UIGrid();
	ref_ptr<UIStackPanel> stackPanel = new UIStackPanel();
	ref_ptr<UIButton> button1 = new UIButton();
	ref_ptr<UIButton> button2 = new UIButton();

	button1->setUIMName("button_endGame");
	button2->setUIMName("button_startGame");
	button1->setText("End Game");
	button2->setText("Start Game");

	stackPanel->getCells()->setNumCells(2);
	//stackPanel->setVerticalAlignment(UIElement::TOP);
	stackPanel->setOrientation(UIStackPanel::VERTICAL);

	stackPanel->addChild(button1, 0);
	stackPanel->addChild(button2, 1);

	grid->setMargin(10.0f);
	grid->setPadding(10.0f);

	grid->getColumns()->setNumCells(2);
	grid->getRows()->setNumCells(2);

	grid->getRows()->setSizePolicy(1, UICells::CONTENT);

	root->addChild(grid);
	grid->addChild(stackPanel, ColRow(0, 1));

	getHud()->registerUserInteractionModel(button1);
	getHud()->registerUserInteractionModel(button2);
}

GameState::StateEvent* MainMenuState::update()
{
	setCameraLatLong(getCameraLatLong() + Vec2f(0.0f, -getFrameTime() * 0.02f));

	return GlobeCameraState::update();
}

void MainMenuState::onUIMClickedEvent(UserInteractionModel* model)
{
	if (model->getUIMName() == "button_endGame")
	{
		stateEvent_endGame();
	}
	else if (model->getUIMName() == "button_startGame")
	{
		stateEvent_replace(new GlobeInteractionState());
	}
}

ref_ptr<Hud> MainMenuState::newHud()
{
	return new Hud();
}