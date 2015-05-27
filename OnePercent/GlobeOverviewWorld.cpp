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
	_simulation = new Simulation();

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

ref_ptr<Simulation> GlobeOverviewWorld::getSimulation()
{
	return _simulation;
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

Vec3f GlobeOverviewWorld::setTimeOfYearAndDay(Vec2f timeOfYearAndDay)
{
	_timeOfYearAndDay = timeOfYearAndDay;

	Matrix yearMat = getMatrixFromEuler(0.0f, 0.0f, -_timeOfYearAndDay.x() * 2.0f * C_PI) *
		getMatrixFromEuler(-sin(_timeOfYearAndDay.x() * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
	Matrix dayMat = getMatrixFromEuler(0.0f, 0.0f, _timeOfYearAndDay.y() * 2.0f * C_PI);

	Matrix yearDayMat = dayMat * yearMat;

	Vec3f direction = yearDayMat * Vec3f(0.0f, 1.0f, 0.0f);

	getLight(0)->setPosition(Vec4f(direction, 0.0f));
	_globeModel->updateLightDirection(direction);

	_backgroundModel->getTransform()->setAttitude(Matrix::inverse(dayMat).getRotate());
	_backgroundModel->getSunTransform()->setAttitude(Matrix::inverse(yearMat).getRotate());

	return direction;
}

void GlobeOverviewWorld::updateSun(Vec3f sunDirection)
{
	Vec3f look = getCameraManipulator()->getLookDirection();
	// look to sun
	if (look * sunDirection < 0.0f)
	{
		float range = 0.3f;

		float dist = pointLineDistance(getCameraManipulator()->getPosition(), sunDirection, Vec3f(0.0f, 0.0f, 0.0f));
		float scale = clamp<float>(dist - GlobeModel::EARTH_RADIUS, 0.0f, range);
		scale /= range;
		scale *= 2.5f;

		_backgroundModel->getSunGlowTransform()->setScale(Vec3f(scale, scale, scale));
	}
}