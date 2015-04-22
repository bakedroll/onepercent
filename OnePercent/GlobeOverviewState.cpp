#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGA;
using namespace osgGaming;

GlobeOverviewState::GlobeOverviewState()
	: GameState(),
	  _globeAngle(0.0)
{

}

void GlobeOverviewState::initialize(ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	_globeWorld = static_cast<GlobeOverviewWorld*>(world.get());
}

StateEvent* GlobeOverviewState::update(double frameTime, ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	_globeAngle = fmodf(_globeAngle + (float)frameTime * 0.1f, (C_PI * 2.0f));
	_globeWorld->getGlobeTransform()->setAttitude(getQuatFromEuler(0.0, 0.0, _globeAngle));

	return stateEvent_default();
}

void GlobeOverviewState::onKeyPressedEvent(int key)
{
	if (key == GUIEventAdapter::KEY_Escape)
	{
		stateEvent_pop();
	}
}

void GlobeOverviewState::onMousePressedEvent(int button, float x, float y)
{
	Vec3f point, direction;
	_globeWorld->getCameraManipulator()->getPickRay(x, y, point, direction);

	printf("Pressed mouse button: %d at %f, %f; Point: %f, %f, %f; Direction: %f, %f, %f\n", button, x, y,
		point.x(), point.y(), point.z(),
		direction.x(), direction.y(), direction.z());

	Vec3f pickResult;
	if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), 6.371f, point, direction, pickResult))
	{
		printf("INTERSECTION\n");
	}
}

void GlobeOverviewState::onDragEvent(int button, Vec2f origin, Vec2f position)
{
	printf("onDragEvent - button: %d; origin: %f, %f; position: %f, %f\n",
		button,
		origin.x(), origin.y(),
		position.x(), position.y());
}