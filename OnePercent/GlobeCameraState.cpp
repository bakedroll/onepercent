#include "GlobeCameraState.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace std;
using namespace osgGA;
using namespace osgGaming;

GlobeCameraState::GlobeCameraState()
	: GameState()
{
}

void GlobeCameraState::initialize()
{
	_globeWorld = static_cast<GlobeOverviewWorld*>(getWorld().get());

	_cameraLatLong = _globeWorld->getCameraLatLong();
	_cameraDistance = _globeWorld->getCameraDistance();
	_cameraViewAngle = _globeWorld->getCameraViewAngle();

	_cameraLatLongAnimation = new Animation<Vec2f>(_cameraLatLong, 0.5, CIRCLE_OUT);
	_cameraDistanceAnimation = new Animation<float>(_cameraDistance, 0.5, CIRCLE_OUT);
	_cameraViewAngleAnimation = new Animation<Vec2f>(_cameraViewAngle, 0.5, CIRCLE_OUT);

	_globeWorld->getBackgroundModel()->updateResolutionHeight(getViewer()->getResolution().y());
}

GameState::StateEvent* GlobeCameraState::update()
{
	_globeWorld->updateCameraPosition(
		_cameraLatLongAnimation->getValue(getSimulationTime()),
		_cameraViewAngleAnimation->getValue(getSimulationTime()),
		_cameraDistanceAnimation->getValue(getSimulationTime()));

	return stateEvent_default();
}

ref_ptr<GlobeOverviewWorld> GlobeCameraState::getGlobeOverviewWorld()
{
	return _globeWorld;
}

Vec2f GlobeCameraState::getCameraLatLong()
{
	return _cameraLatLong;
}

float GlobeCameraState::getCameraDistance()
{
	return _cameraDistance;
}

Vec2f GlobeCameraState::getCameraViewAngle()
{
	return _cameraViewAngle;
}

void GlobeCameraState::setCameraLatLong(osg::Vec2f latLong, double time)
{
	_cameraLatLong = latLong;

	if (time < 0.0)
	{
		_cameraLatLongAnimation->setValue(_cameraLatLong);
	}
	else
	{
		_cameraLatLongAnimation->beginAnimation(_cameraLatLong, time);
	}
}

void GlobeCameraState::setCameraDistance(float distance, double time)
{
	_cameraDistance = distance;

	if (time < 0.0)
	{
		_cameraDistanceAnimation->setValue(_cameraDistance);
	}
	else
	{
		_cameraDistanceAnimation->beginAnimation(_cameraDistance, time);
	}
}

void GlobeCameraState::setCameraViewAngle(osg::Vec2f viewAngle, double time)
{
	_cameraViewAngle = viewAngle;

	if (time < 0.0)
	{
		_cameraViewAngleAnimation->setValue(_cameraViewAngle);
	}
	else
	{
		_cameraViewAngleAnimation->beginAnimation(_cameraViewAngle, time);
	}
}
