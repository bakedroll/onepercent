#include <osgGaming/GameApplication.h>
#include <osgGaming/ResourceManager.h>

#include "LoadingGlobeOverviewState.h"
#include "GlobeOverviewState.h"
#include "GlobeOverviewWorld.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

int main(int argc, char** argv)
{
	ResourceManager::getInstance()->setDefaultFontResourceKey("./data/fonts/coolvetica rg.ttf");

	ref_ptr<GameApplication> app = new GameApplication();
	app->setWorld<GlobeOverviewWorld>();

	//app->getGameSettings()->setScreenNum(1);
	//app->getGameSettings()->setFullscreenEnabled(true);

	return app->run(new LoadingGlobeOverviewState(new GlobeOverviewState()));
}
