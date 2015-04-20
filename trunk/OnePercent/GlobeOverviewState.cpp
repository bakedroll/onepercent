#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGA;
using namespace osgGaming;

GlobeOverviewState::GlobeOverviewState()
	: GameState(),
	  _globeAngle(0.0),
	  _exitGame(false)
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

	if (_exitGame)
	{
		return stateEvent_endGame();
	}

	return 0;
}

void GlobeOverviewState::onKeyHitEvent(int key)
{
	if (key == GUIEventAdapter::KEY_Escape)
	{
		_exitGame = true;
	}
}

void GlobeOverviewState::onMouseHitEvent(int button, float x, float y)
{
	Vec3f point, direction;

	_globeWorld->getCameraManipulator()->getPickVector(x, y, point, direction);

	printf("Pressed mouse button: %d at %f, %f; Point: %f, %f, %f; Direction: %f, %f, %f\n", button, x, y,
		point.x(), point.y(), point.z(),
		direction.x(), direction.y(), direction.z());

	Vec3f pickResult;
	if (sphereLineIntersection(Vec3f(0.0f, 0.0f, 0.0f), 6.371f, point, direction, pickResult))
	{
		printf("INTERSECTION\n");
	}
}