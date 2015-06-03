#include "LoadingGlobeOverviewState.h"
#include "GlobeModel.h"
#include "GlobeOverviewWorld.h"
#include "BackgroundModel.h"

#include <osg/PositionAttitudeTransform>
#include <osg/StateSet>
#include <osg/Program>
#include <osgGaming/Helper.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UIText.h>
#include <osgGaming/UIButton.h>

#include <osgGaming/PackageResourceLoader.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;
using namespace osgText;
using namespace std;

LoadingGlobeOverviewState::LoadingGlobeOverviewState(ref_ptr<GameState> nextState)
	: GameLoadingState(nextState)
{

}

void LoadingGlobeOverviewState::initialize()
{
	float projNear = (float)getWorld()->getCameraManipulator()->getProjectionNear();
	float projFar = (float)getWorld()->getCameraManipulator()->getProjectionFar();

	_loadingText = new UIText();
	_loadingText->setText("Loading");
	_loadingText->setFontSize(25);
	_loadingText->setVerticalAlignment(UIElement::BOTTOM);
	_loadingText->setMargin(10.0f);

	getWorld()->getHud()->getRootUIElement()->addChild(_loadingText);

	getViewer()->setClampColorEnabled(true);
	getViewer()->addPostProcessingEffect(new HighDynamicRangeEffect());
	getViewer()->addPostProcessingEffect(new DepthOfFieldEffect(projNear, projFar), false);
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
	ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());


	globeWorld->getSimulation()->loadCountries();


	ref_ptr<GlobeModel> globe = new GlobeModel(world->getCameraManipulator());
	globeWorld->setGlobeModel(globe);
	world->getRootNode()->addChild(globe);

	ref_ptr<BackgroundModel> backgroundModel = new BackgroundModel();
	globeWorld->setBackgroundModel(backgroundModel);
	world->getRootNode()->addChild(backgroundModel->getTransform());

	world->getHud()->setFpsEnabled(true);



	// ##################
	/*ref_ptr<UIElement> root = world->getHud()->getRootUIElement();

	ref_ptr<UIGrid> grid = new UIGrid();
	ref_ptr<UIText> first = new UIText();
	ref_ptr<UIText> second = new UIText();
	ref_ptr<UIText> third = new UIText();
	ref_ptr<UIStackPanel> stackPanel = new UIStackPanel();
	ref_ptr<UIButton> button1 = new UIButton();
	ref_ptr<UIButton> button2 = new UIButton();

	button1->setUIMName("button1");
	button2->setUIMName("button2");
	button1->setText("button 1");
	button2->setText("button 2");

	first->setText("Text1");
	second->setText("Text2");
	third->setText("Text3");

	first->setPadding(20.0f);
	first->setTextAlignment(osgText::TextBase::RIGHT_TOP);

	stackPanel->getCells()->setNumCells(2);
	stackPanel->setVerticalAlignment(UIElement::BOTTOM);
	
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
	world->getHud()->registerUserInteractionModel(button2);*/
	// #############
}