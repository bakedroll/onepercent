#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;

GlobeOverviewWorld::GlobeOverviewWorld()
	: World(),
	 _timeOfYearAndDay(0.0f, 0.0f)
{
	getGlobalLightModel()->setAmbientIntensity(Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	setLightEnabled(0, true);

	ref_ptr<Light> light = getLight(0);

	light->setDiffuse(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setSpecular(Vec4(1.0, 1.0, 1.0, 1.0));
	light->setAmbient(Vec4(0.0, 0.0, 0.0, 1.0));
}

Vec2f GlobeOverviewWorld::getTimeOfYearAndDay()
{
	return _timeOfYearAndDay;
}

void GlobeOverviewWorld::setTimeOfYearAndDay(Vec2f timeOfYearAndDay)
{
	_timeOfYearAndDay = timeOfYearAndDay;

	Matrix yearMat = getMatrixFromEuler(sin(_timeOfYearAndDay.x() * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
	Matrix dayMat = getMatrixFromEuler(0.0f, 0.0f, _timeOfYearAndDay.y() * 2.0f * C_PI);

	getLight(0)->setPosition(dayMat * yearMat * Vec4(0.0f, 1.0f, 0.0f, 0.0f));
}