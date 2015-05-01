#include "LoadingGlobeOverviewState.h"
#include "GlobeModel.h"
#include "GlobeOverviewWorld.h"

#include <osg/PositionAttitudeTransform>
#include <osgGaming/Helper.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UIText.h>
#include <osgGaming/UIButton.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;
using namespace std;

LoadingGlobeOverviewState::LoadingGlobeOverviewState(ref_ptr<GameState> nextState)
	: GameLoadingState(nextState)
{

}

void LoadingGlobeOverviewState::initialize()
{
	_loadingText = new osgText::Text();
	_loadingText->setCharacterSize(25);
	_loadingText->setFont("./data/fonts/coolvetica rg.ttf");
	_loadingText->setText("");
	_loadingText->setAxisAlignment(osgText::Text::SCREEN);
	_loadingText->setAlignment(osgText::TextBase::LEFT_BOTTOM);
	_loadingText->setPosition(osg::Vec3(10, 10, -1.5));
	_loadingText->setColor(osg::Vec4(199, 77, 15, 1));
	_loadingText->setDataVariance(osg::Object::DYNAMIC);

	getWorld()->getHud()->getGeode()->addDrawable(_loadingText);
}

StateEvent* LoadingGlobeOverviewState::update()
{
	int dotCount = (int)(getSimulationTime() * 10.0) % 4;
	string loadingTextString = "Loading";
	for (int i = 0; i < dotCount; i++)
		loadingTextString += ".";
		
	_loadingText->setText(loadingTextString);

	return stateEvent_default();
}

void LoadingGlobeOverviewState::load(ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	// ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

	ref_ptr<GlobeModel> globe = new GlobeModel();

	world->getRootNode()->addChild(globe);
	world->getHud()->setFpsEnabled(true);



	ref_ptr<UIElement> root = world->getHud()->getRootUIElement();

	ref_ptr<UIGrid> grid = new UIGrid();
	ref_ptr<UIText> first = new UIText();
	ref_ptr<UIText> second = new UIText();
	ref_ptr<UIText> third = new UIText();
	ref_ptr<UIStackPanel> stackPanel = new UIStackPanel();
	ref_ptr<UIButton> button1 = new UIButton();
	ref_ptr<UIButton> button2 = new UIButton();



	first->setText("Text1");
	second->setText("Text2");
	third->setText("Text3");

	first->setPadding(20.0f);
	first->setTextAlignment(osgText::TextBase::RIGHT_TOP);

	stackPanel->getCells()->setNumCells(2);
	
	stackPanel->addChild(button1, 0);
	stackPanel->addChild(button2, 1);

	grid->setMargin(10.0f);
	grid->setPadding(10.0f);

	grid->getColumns()->setNumCells(3);
	grid->getRows()->setNumCells(2);

	grid->getColumns()->setSizePolicy(1, UICells::CONTENT);

	root->addChild(grid);
	grid->addChild(first, ColRow(0, 0));
	grid->addChild(second, ColRow(1, 0));
	grid->addChild(third, ColRow(0, 1));
	grid->addChild(stackPanel, ColRow(2, 0));


	root->getVisualGroup();
	grid->getVisualGroup();
	first->getVisualGroup();
	second->getVisualGroup();
	third->getVisualGroup();

	world->getHud()->registerUserInteractionModel(button1);
	world->getHud()->registerUserInteractionModel(button2);


	world->getHud()->updateUIElements();
	Vec2f origin = button1->getAbsoluteOrigin();

	return;
}