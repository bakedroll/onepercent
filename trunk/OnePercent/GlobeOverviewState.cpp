#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;

GlobeOverviewState::GlobeOverviewState()
	: GameState(),
	  _globeAngle(0.0)
{

}

StateEvent* GlobeOverviewState::update(double frameTime, ref_ptr<World> world, ref_ptr<GameSettings> settings)
{
	ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

	_globeAngle = fmodf(_globeAngle + (float)frameTime * 0.1f, (C_PI * 2.0f));
	globeWorld->getGlobeTransform()->setAttitude(getQuatFromEuler(0.0, 0.0, _globeAngle));

	return 0;
}