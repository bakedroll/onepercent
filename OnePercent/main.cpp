#include <osgGaming/GameApplication.h>

#include "LoadingGlobeOverviewState.h"
#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

int main(int argc, char** argv)
{
	ref_ptr<GameApplication> app = new GameApplication();
	app->setWorld<GlobeOverviewWorld>();

	return app->run(new LoadingGlobeOverviewState(new GlobeOverviewState()));
}
