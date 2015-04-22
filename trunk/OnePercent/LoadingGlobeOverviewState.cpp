#include "LoadingGlobeOverviewState.h"
#include "GlobeModel.h"
#include "GlobeOverviewWorld.h"

#include <osg/PositionAttitudeTransform>
#include <osgGaming/Helper.h>

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
	int dotCount = (int)(getSimulationTime() * 4.0) % 4;
	string loadingTextString = "Loading";
	for (int i = 0; i < dotCount; i++)
		loadingTextString += ".";
		
	_loadingText->setText(loadingTextString);

	return stateEvent_default();
}

void LoadingGlobeOverviewState::load(ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

	ref_ptr<GlobeModel> globe = new GlobeModel();

	globeWorld->getRootNode()->addChild(globe);
}