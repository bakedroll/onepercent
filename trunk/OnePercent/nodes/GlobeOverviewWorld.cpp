#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"
#include "BackgroundModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;

GlobeOverviewWorld::GlobeOverviewWorld()
	: World(),
	  _cameraLatLong(Vec2f(0.0f, 0.0f)),
	  _cameraViewAngle(Vec2f(0.0f, 0.0f)),
	  _cameraDistance(28.0f)
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

ref_ptr<CountryOverlay> GlobeOverviewWorld::getCountryOverlay()
{
	return _countryOverlay;
}

ref_ptr<BackgroundModel> GlobeOverviewWorld::getBackgroundModel()
{
	return _backgroundModel;
}

ref_ptr<Simulation> GlobeOverviewWorld::getSimulation()
{
	return _simulation;
}

void GlobeOverviewWorld::setGlobeModel(ref_ptr<GlobeModel> globeModel)
{
	if (_globeModel.valid())
	{
		getRootNode()->removeChild(_globeModel);
	}

	getRootNode()->addChild(globeModel);
	_globeModel = globeModel;
}

void GlobeOverviewWorld::setCountryOverlay(osg::ref_ptr<CountryOverlay> countryOverlay)
{
	if (_countryOverlay.valid())
	{
		getRootNode()->removeChild(_countryOverlay);
	}

	getRootNode()->addChild(countryOverlay);
	_countryOverlay = countryOverlay;
}

void GlobeOverviewWorld::setBackgroundModel(ref_ptr<BackgroundModel> backgroundModel)
{
	if (_backgroundModel.valid())
	{
		getRootNode()->removeChild(_backgroundModel->getTransform());
	}

	getRootNode()->addChild(backgroundModel->getTransform());
	_backgroundModel = backgroundModel;
}

void GlobeOverviewWorld::setDay(float day)
{
	float year = day / ~Property<float, Param_MechanicsDaysInYearName>();

	Matrix yearMat = getMatrixFromEuler(0.0f, 0.0f, - year * 2.0f * C_PI) *
		getMatrixFromEuler(-sin(year * 2.0f * C_PI) * 23.5f * C_PI / 180.0f, 0.0f, 0.0f);
	Matrix dayMat = getMatrixFromEuler(0.0f, 0.0f, day * 2.0f * C_PI);

	Matrix yearDayMat = dayMat * yearMat;

	Vec3f direction = yearDayMat * Vec3f(0.0f, 1.0f, 0.0f);

	getLight(0)->setPosition(Vec4f(direction, 0.0f));
	_globeModel->updateLightDirection(direction);

	_backgroundModel->getTransform()->setAttitude(Matrix::inverse(dayMat).getRotate());
	_backgroundModel->getSunTransform()->setAttitude(Matrix::inverse(yearMat).getRotate());

	_globeModel->updateClouds(day);
	updateSun(direction);
}

Vec2f GlobeOverviewWorld::getCameraLatLong()
{
	return _cameraLatLong;
}

Vec2f GlobeOverviewWorld::getCameraViewAngle()
{
	return _cameraViewAngle;
}

float GlobeOverviewWorld::getCameraDistance()
{
	return _cameraDistance;
}

void GlobeOverviewWorld::updateCameraPosition(Vec2f latLong, Vec2f viewAngle, float distance)
{
	_cameraLatLong = latLong;
	_cameraViewAngle = viewAngle;
	_cameraDistance = distance;

	// update camera position
	Vec3f position = getVec3FromEuler(latLong.x(), 0.0, latLong.y()) * _cameraDistance;

	getCameraManipulator()->setPosition(position);

	_backgroundModel->getTransform()->setPosition(position);

	// update camera attitude
	Matrix latLongMat = Matrix::rotate(getQuatFromEuler(-latLong.x(), 0.0f, fmodf(latLong.y() + C_PI, C_PI * 2.0f)));
	Matrix viewAngleMat = Matrix::rotate(getQuatFromEuler(viewAngle.y(), viewAngle.x(), 0.0f));

	Matrix mat = viewAngleMat * latLongMat;

	getCameraManipulator()->setAttitude(mat.getRotate());
}

void GlobeOverviewWorld::updateSun(Vec3f sunDirection)
{
	Vec3f look = getCameraManipulator()->getLookDirection();
	// look to sun
	if (look * sunDirection < 0.0f)
	{
		float range = 0.3f;

		float dist = pointLineDistance(getCameraManipulator()->getPosition(), sunDirection, Vec3f(0.0f, 0.0f, 0.0f));
		float scale = clampBetween(dist - ~_paramEarthRadius, 0.0f, range);
		scale /= range;
		scale *= 2.5f;

		_backgroundModel->getSunGlowTransform()->setScale(Vec3f(scale, scale, scale));
	}
}