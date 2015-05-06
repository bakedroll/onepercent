#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"
#include "BackgroundModel.h"

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

ref_ptr<GlobeModel> GlobeOverviewWorld::getGlobeModel()
{
	return _globeModel;
}

ref_ptr<BackgroundModel> GlobeOverviewWorld::getBackgroundModel()
{
	return _backgroundModel;
}

Vec2f GlobeOverviewWorld::getTimeOfYearAndDay()
{
	return _timeOfYearAndDay;
}

void GlobeOverviewWorld::setGlobeModel(osg::ref_ptr<GlobeModel> globeModel)
{
	_globeModel = globeModel;
}

void GlobeOverviewWorld::setBackgroundModel(ref_ptr<BackgroundModel> backgroundModel)
{
	_backgroundModel = backgroundModel;
}

void GlobeOverviewWorld::setTimeOfYearAndDay(Vec2f timeOfYearAndDay)
{
	_timeOfYearAndDay = timeOfYearAndDay;

	// printf("Year: %f, Day: %f\n", _timeOfYearAndDay.x(), _timeOfYearAndDay.y());

	Matrix yearMat = getMatrixFromEuler(sin(_timeOfYearAndDay.x() * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
	Matrix dayMat = getMatrixFromEuler(0.0f, 0.0f, _timeOfYearAndDay.y() * 2.0f * C_PI);

	Matrix yearDayMat = dayMat * yearMat;

	Vec4f direction = yearDayMat * Vec4(0.0f, -1.0f, 0.0f, 0.0f);

	getLight(0)->setPosition(direction);
	_globeModel->updateLightDirection(Vec3f(direction.x(), direction.y(), direction.z()));

	//_backgroundModel->setAttitude(yearDayMat.getRotate());
}