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
	: GlobeCameraState()
{
}

void GlobeInteractionState::initialize()
{
	GlobeCameraState::initialize();

	setCameraDistance(28.0f, getSimulationTime());
	setCameraViewAngle(Vec2f(0.0f, 0.0f), getSimulationTime());
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
			unsigned char id = getGlobeOverviewWorld()->getSimulation()->getCountryId(polar);
			string country_name = getGlobeOverviewWorld()->getSimulation()->getCountryName(polar);

			getGlobeOverviewWorld()->getGlobeModel()->setSelectedCountry((int)id);

			printf("INTERSECTION at %f, %f, %f Polar: %f, %f Id: %d Country: %s\n", pickResult.x(), pickResult.y(), pickResult.z(), polar.x(), polar.y(), (int)id, country_name.data());
		}
	}
}

void GlobeInteractionState::onScrollEvent(GUIEventAdapter::ScrollingMotion motion)
{
	float distance = getCameraDistance();

	if (motion == GUIEventAdapter::SCROLL_UP)
	{
		distance = distance * _CAMERA_ZOOM_SPEED;
	}
	else if (motion == GUIEventAdapter::SCROLL_DOWN)
	{
		distance = distance * (1.0f / _CAMERA_ZOOM_SPEED);
	}
	else
	{
		return;
	}

	distance = clamp(distance, _MIN_CAMERA_DISTANCE, _MAX_CAMERA_DISTANCE);

	setCameraDistance(distance, getSimulationTime());
}

void GlobeInteractionState::onDragEvent(int button, Vec2f origin, Vec2f position, osg::Vec2f change)
{
	Vec2f latLong = getCameraLatLong();
	float distance = getCameraDistance();
	Vec2f viewAngle = getCameraViewAngle();

	if (button == GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		change *= ((distance - GlobeModel::EARTH_RADIUS) / (_MAX_CAMERA_DISTANCE - GlobeModel::EARTH_RADIUS)) * _CAMERA_ZOOM_SPEED_FACTOR;

		latLong.set(
			clamp(latLong.x() - change.y() * _CAMERA_SCROLL_SPEED, -_MAX_CAMERA_LONGITUDE, _MAX_CAMERA_LONGITUDE),
			latLong.y() - change.x() * _CAMERA_SCROLL_SPEED);

		setCameraLatLong(latLong, getSimulationTime());
	}
	else if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		float clamp_to = atan(GlobeModel::EARTH_RADIUS * 1.3 / distance);

		viewAngle.set(
			viewAngle.x() + change.x() * _CAMERA_ROTATION_SPEED,
			clamp(viewAngle.y() + change.y() * _CAMERA_ROTATION_SPEED, 0.0f, clamp_to));

		setCameraViewAngle(viewAngle, getSimulationTime());
	}
}

void GlobeInteractionState::onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position)
{
	if (button == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		setCameraViewAngle(Vec2f(0.0f, 0.0f), getSimulationTime());
	}
}

/*void GlobeInteractionState::onUIMClickedEvent(UserInteractionModel* model)
{
	printf("Clicked UIM: %s\n", model->getUIMName().data());
}*/

ref_ptr<Hud> GlobeInteractionState::newHud()
{
	return new Hud();
}