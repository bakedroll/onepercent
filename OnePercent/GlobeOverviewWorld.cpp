#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;

void GlobeOverviewWorld::initialize()
{
	_globeTransform = new PositionAttitudeTransform();
	ref_ptr<GlobeModel> globe = new GlobeModel();

	_globeTransform->addChild(globe);
	getRootNode()->addChild(_globeTransform);

	getGlobalLightModel()->setAmbientIntensity(Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	setLightEnabled(0, true);

	ref_ptr<Light> light = getLight(0);

	light->setDiffuse(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
	light->setPosition(Vec4(getVec3FromEuler(0.0, -23.5 * C_PI / 180.0, C_PI / 2.0), 0.0));

}

ref_ptr<PositionAttitudeTransform> GlobeOverviewWorld::getGlobeTransform()
{
	return _globeTransform;
}