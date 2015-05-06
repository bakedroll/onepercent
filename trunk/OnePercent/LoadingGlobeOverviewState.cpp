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
	_loadingText = new UIText();
	_loadingText->setText("Loading");
	_loadingText->setFontSize(25);
	_loadingText->setVerticalAlignment(UIElement::BOTTOM);
	_loadingText->setMargin(10.0f);

	getWorld()->getHud()->getRootUIElement()->addChild(_loadingText);
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

	ref_ptr<GlobeModel> globe = new GlobeModel(world->getCameraManipulator());
	globeWorld->setGlobeModel(globe);
	world->getRootNode()->addChild(globe);

	ref_ptr<BackgroundModel> backgroundModel = new BackgroundModel("./GameData/data/stars.bin");
	globeWorld->setBackgroundModel(backgroundModel);
	world->getRootNode()->addChild(backgroundModel);

	world->getHud()->setFpsEnabled(true);

	/*ref_ptr<CameraAlignedQuad> caq = new CameraAlignedQuad(-1);
	
	world->getRootNode()->addChild(caq);
	world->getCameraManipulator()->addCameraAlignedQuad(caq);

	// shader
	ref_ptr<StateSet> stateSet = caq->getOrCreateStateSet();

	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/stars.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/stars.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	stateSet->setAttribute(pgm, StateAttribute::ON);

	TextureFactory::make()
		->image(ResourceManager::getInstance()->loadImage("./GameData/textures/stars/8k/0x0.png"))
		->texLayer(0)
		->uniform(stateSet, "colormap_0x0")
		->assign(stateSet)
		->build();

	TextureFactory::make()
		->image(ResourceManager::getInstance()->loadImage("./GameData/textures/stars/8k/1x0.png"))
		->texLayer(1)
		->uniform(stateSet, "colormap_1x0")
		->assign(stateSet)
		->build();*/



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