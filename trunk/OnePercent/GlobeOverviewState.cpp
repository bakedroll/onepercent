#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGA;
using namespace osgGaming;

const float GlobeOverviewState::_MIN_CAMERA_DISTANCE = 9.0f;
const float GlobeOverviewState::_MAX_CAMERA_DISTANCE = 100.0f;
const float GlobeOverviewState::_MAX_CAMERA_LONGITUDE = C_PI / 2.0f * 0.9f;
const float GlobeOverviewState::_CAMERA_ZOOM_SPEED = 0.85f;
const float GlobeOverviewState::_CAMERA_ZOOM_SPEED_FACTOR = 3.0f;
const float GlobeOverviewState::_CAMERA_SCROLL_SPEED = 0.003f;
const float GlobeOverviewState::_DAYS_IN_YEAR = 356.0f;
const float GlobeOverviewState::_TIME_SPEED = 0.02f;

GlobeOverviewState::GlobeOverviewState()
	: GameState(),
	  _cameraLatLong(Vec2f(0.0f, 0.0f)),
	  _cameraDistance(28.0f)
{
	_cameraLatLongAnimation = new Animation<Vec2f>(_cameraLatLong, 0.5, CIRCLE_OUT);
	_cameraDistanceAnimation = new Animation<float>(_cameraDistance, 0.5, CIRCLE_OUT);
}

void GlobeOverviewState::initialize()
{
	_globeWorld = static_cast<GlobeOverviewWorld*>(getWorld().get());
}

StateEvent* GlobeOverviewState::update()
{
	// update camera position
	Vec2f latLong = _cameraLatLongAnimation->getValue(getSimulationTime());

	getWorld()->getCameraManipulator()->setPosition(
		getVec3FromEuler(latLong.x(), 0.0, latLong.y())
			* _cameraDistanceAnimation->getValue(getSimulationTime()));

	getWorld()->getCameraManipulator()->setAttitude(getQuatFromEuler(-latLong.x(), 0.0, fmodf(latLong.y() + C_PI, C_PI * 2.0f)));

	// update light direction
	Vec2f yearDay = _globeWorld->getTimeOfYearAndDay();

	yearDay.x() = fmodf(yearDay.x() + getFrameTime() * _TIME_SPEED / _DAYS_IN_YEAR, 1.0);
	yearDay.y() = fmodf(yearDay.y() + getFrameTime() * _TIME_SPEED, 1.0);

	_globeWorld->setTimeOfYearAndDay(yearDay);

	return stateEvent_default();
}

void GlobeOverviewState::onKeyPressedEvent(int key)
{
	if (key == GUIEventAdapter::KEY_Escape)
	{
		stateEvent_pop();
	}
	else if (key == GUIEventAdapter::KEY_0)
	{
		printf("bla\n");
		getGameSettings()->setFullscreenEnabled(!getGameSettings()->getFullscreenEnabled());
	}
}

void GlobeOverviewState::onMousePressedEvent(int button, float x, float y)
{
	if (button == GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		Vec3f point, direction;
		getWorld()->getCameraManipulator()->getPickRay(x, y, point, direction);

		/*printf("Pressed mouse button: %d at %f, %f; Point: %f, %f, %f; Direction: %f, %f, %f\n", button, x, y,
			point.x(), point.y(), point.z(),
			direction.x(), direction.y(), direction.z());*/

		Vec3f pickResult;
		if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), 6.371f, point, direction, pickResult))
		{
			printf("INTERSECTION at %f, %f, %f\n", pickResult.x(), pickResult.y(), pickResult.z());
		}
	}
}

void GlobeOverviewState::onScrollEvent(GUIEventAdapter::ScrollingMotion motion)
{
	if (motion == GUIEventAdapter::SCROLL_UP)
	{
		_cameraDistance = _cameraDistance * _CAMERA_ZOOM_SPEED;
	}
	else if (motion == GUIEventAdapter::SCROLL_DOWN)
	{
		_cameraDistance = _cameraDistance * (1.0f / _CAMERA_ZOOM_SPEED);
	}
	else
	{
		return;
	}

	_cameraDistance = clamp(_cameraDistance, _MIN_CAMERA_DISTANCE, _MAX_CAMERA_DISTANCE);

	_cameraDistanceAnimation->beginAnimation(_cameraDistance, getSimulationTime());
}

void GlobeOverviewState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
{
	if (button == GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		change *= ((_cameraDistance - GlobeModel::EARTH_RADIUS) / (_MAX_CAMERA_DISTANCE - GlobeModel::EARTH_RADIUS)) * _CAMERA_ZOOM_SPEED_FACTOR;

		_cameraLatLong.set(
			clamp(_cameraLatLong.x() - change.y() * _CAMERA_SCROLL_SPEED, -_MAX_CAMERA_LONGITUDE, _MAX_CAMERA_LONGITUDE),
			_cameraLatLong.y() - change.x() * _CAMERA_SCROLL_SPEED);

		_cameraLatLongAnimation->beginAnimation(_cameraLatLong, getSimulationTime());
	}
}