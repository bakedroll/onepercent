#include <osgGaming/GameApplication.h>

#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

int main(int argc, char** argv)
{
	ref_ptr<GameApplication> app = new GameApplication();

	ref_ptr<GlobeOverviewWorld> world = new GlobeOverviewWorld();
	world->initialize();

	app->setWorld(world);

	return app->run(new GlobeOverviewState());
}
