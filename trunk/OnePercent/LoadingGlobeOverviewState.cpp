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
	: GameLoadingState(nextState),
	_elapsedTime(0.0)
{

}

void LoadingGlobeOverviewState::initialize(osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings)
{
	_loadingText = new osgText::Text();
	_loadingText->setCharacterSize(25);
	// textOne->setFont("C:/WINDOWS/Fonts/impact.ttf");
	_loadingText->setText("");
	_loadingText->setAxisAlignment(osgText::Text::SCREEN);
	_loadingText->setAlignment(osgText::TextBase::LEFT_BOTTOM);
	_loadingText->setPosition(osg::Vec3(10, 10, -1.5));
	_loadingText->setColor(osg::Vec4(199, 77, 15, 1));
	_loadingText->setDataVariance(osg::Object::DYNAMIC);

	world->getHud()->getGeode()->addDrawable(_loadingText);
}

StateEvent* LoadingGlobeOverviewState::update(double frameTime, osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings)
{
	_elapsedTime += frameTime;

	int dotCount = (int)(_elapsedTime * 4.0) % 4;
	string loadingTextString = "Loading";
	for (int i = 0; i < dotCount; i++)
		loadingTextString += ".";
		
	_loadingText->setText(loadingTextString);

	return 0;
}

void LoadingGlobeOverviewState::load(ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

	ref_ptr<PositionAttitudeTransform> transform = new PositionAttitudeTransform();
	ref_ptr<GlobeModel> globe = new GlobeModel();

	transform->addChild(globe);


	globeWorld->setGlobeTransform(transform);
	globeWorld->getGlobalLightModel()->setAmbientIntensity(Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	globeWorld->setLightEnabled(0, true);

	ref_ptr<Light> light = globeWorld->getLight(0);

	light->setDiffuse(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
	light->setPosition(Vec4(getVec3FromEuler(0.0, -23.5 * C_PI / 180.0, C_PI / 2.0), 0.0));
}