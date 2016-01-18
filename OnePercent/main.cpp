#include <osgGaming/GameApplication.h>
#include <osgGaming/ResourceManager.h>

#include "LoadingGlobeOverviewState.h"
#include "GlobeOverviewState.h"
#include "MainMenuState.h"
#include "GlobeOverviewWorld.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

int main(int argc, char** argv)
{
	/*ref_ptr<PackageResourceLoader> resourceLoader = new PackageResourceLoader();
	resourceLoader->registerPackage("./GameData/Data.pak");
	resourceLoader->registerPackage("./GameData/Shader.pak");

	ResourceManager::getInstance()->setResourceLoader(resourceLoader);*/
	ResourceManager::getInstance()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");


	ref_ptr<GameApplication> app = new GameApplication();

	app->setDefaultWorld(new GlobeOverviewWorld());

	//app->getDefaultGameSettings()->setScreenNum(1);
	//app->getDefaultGameSettings()->setFullscreenEnabled(true);

	GameState::AbstractGameStateList states;
	states.push_back(new GlobeOverviewState());
	states.push_back(new MainMenuState());

	return app->run(new LoadingGlobeOverviewState(states));
}