#include "GlobeInteractionState.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace std;
using namespace osgGA;
using namespace osgGaming;

const float GlobeInteractionState::_MIN_CAMERA_DISTANCE = 9.0f;
const float GlobeInteractionState::_MAX_CAMERA_DISTANCE = 100.0f;
const float GlobeInteractionState::_MAX_CAMERA_LONGITUDE = C_PI / 2.0f * 0.9f;
const float GlobeInteractionState::_CAMERA_ZOOM_SPEED = 0.85f;
const float GlobeInteractionState::_CAMERA_ZOOM_SPEED_FACTOR = 3.0f;
const float GlobeInteractionState::_CAMERA_SCROLL_SPEED = 0.003f;
const float GlobeInteractionState::_CAMERA_ROTATION_SPEED = 0.003;

GlobeInteractionState::GlobeInteractionState()
	: GameState(),
	_cameraLatLong(Vec2f(0.0f, 0.0f)),
	_cameraDistance(28.0f),
	_cameraViewAngle(Vec2f(0.0f, 0.0f))
{
	_cameraLatLongAnimation = new Animation<Vec2f>(_cameraLatLong, 0.5, CIRCLE_OUT);
	_cameraDistanceAnimation = new Animation<float>(_cameraDistance, 0.5, CIRCLE_OUT);
	_cameraViewAngleAnimation = new Animation<Vec2f>(_cameraViewAngle, 0.5, CIRCLE_OUT);
}

void GlobeInteractionState::initialize()
{
	_globeWorld = static_cast<GlobeOverviewWorld*>(getWorld().get());

	_globeWorld->getBackgroundModel()->updateResolutionHeight(getViewer()->getResolution().y());
}

GameState::StateEvent* GlobeInteractionState::update()
{
	// update camera position
	Vec2f latLong = _cameraLatLongAnimation->getValue(getSimulationTime());
	Vec2f viewAngle = _cameraViewAngleAnimation->getValue(getSimulationTime());

	Vec3f position = getVec3FromEuler(latLong.x(), 0.0, latLong.y())
		* _cameraDistanceAnimation->getValue(getSimulationTime());

	getWorld()->getCameraManipulator()->setPosition(position);

	_globeWorld->getBackgroundModel()->getTransform()->setPosition(position);

	// update camera attitude
	Matrix latLongMat = Matrix::rotate(getQuatFromEuler(-latLong.x(), 0.0f, fmodf(latLong.y() + C_PI, C_PI * 2.0f)));
	Matrix viewAngleMat = Matrix::rotate(getQuatFromEuler(viewAngle.y(), viewAngle.x(), 0.0f));

	Matrix mat = viewAngleMat * latLongMat;

	getWorld()->getCameraManipulator()->setAttitude(mat.getRotate());

	return stateEvent_default();
}

void GlobeInteractionState::onMousePressedEvent(int button, float x, float y)
{
	if (button == GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		Vec3f point, direction;
		getWorld()->getCameraManipulator()->getPickRay(x, y, point, direction);

		Vec3f pickResult;
		if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), GlobeModel::EARTH_RADIUS, point, direction, pickResult))
		{
			Vec2f polar = getPolarFromCartesian(pickResult);
			unsigned char id = _globeWorld->getSimulation()->getCountryId(polar);
			string country_name = _globeWorld->getSimulation()->getCountryName(polar);

			_globeWorld->getGlobeModel()->setSelectedCountry((int)id);

			printf("INTERSECTION at %f, %f, %f Polar: %f, %f Id: %d Country: %s\n", pickResult.x(), pickResult.y(), pickResult.z(), polar.x(), polar.y(), (int)id, country_name.data());
		}
	}
}

void GlobeInteractionState::onScrollEvent(GUIEventAdapter::ScrollingMotion motion)
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

void GlobeInteractionState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
{
	if (button == GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		change *= ((_cameraDistance - GlobeModel::EARTH_RADIUS) / (_MAX_CAMERA_DISTANCE - GlobeModel::EARTH_RADIUS)) * _CAMERA_ZOOM_SPEED_FACTOR;

		_cameraLatLong.set(
			clamp(_cameraLatLong.x() - change.y() * _CAMERA_SCROLL_SPEED, -_MAX_CAMERA_LONGITUDE, _MAX_CAMERA_LONGITUDE),
			_cameraLatLong.y() - change.x() * _CAMERA_SCROLL_SPEED);

		_cameraLatLongAnimation->beginAnimation(_cameraLatLong, getSimulationTime());
	}
	else if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		float clamp_to = atan(GlobeModel::EARTH_RADIUS * 1.3 / _cameraDistance);

		_cameraViewAngle.set(
			_cameraViewAngle.x() + change.x() * _CAMERA_ROTATION_SPEED,
			clamp(_cameraViewAngle.y() + change.y() * _CAMERA_ROTATION_SPEED, 0.0f, clamp_to));

		_cameraViewAngleAnimation->beginAnimation(_cameraViewAngle, getSimulationTime());
	}
}

void GlobeInteractionState::onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position)
{
	if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		_cameraViewAngle.set(0.0f, 0.0f);

		_cameraViewAngleAnimation->beginAnimation(_cameraViewAngle, getSimulationTime());
	}
}

/*void GlobeInteractionState::onUIMClickedEvent(UserInteractionModel* model)
{
	printf("Clicked UIM: %s\n", model->getUIMName().data());
}*/