#include <osgGaming/ResourceManager.h>

#include "core/OnePercentApplication.h"

#include "states/LoadingGlobeOverviewState.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"

#include "nodes/GlobeOverviewWorld.h"

#include "widgets/MainWindow.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

#define INITIALIZE_QT

int main(int argc, char** argv)
{
	/*ref_ptr<PackageResourceLoader> resourceLoader = new PackageResourceLoader();
	resourceLoader->registerPackage("./GameData/Data.pak");
	resourceLoader->registerPackage("./GameData/Shader.pak");

	ResourceManager::getInstance()->setResourceLoader(resourceLoader);*/


#ifdef INITIALIZE_QT

  ref_ptr<OnePercentApplication> app = new OnePercentApplication(argc, argv);
  return app->run();

#else

  ref_ptr<GameApplication> app = new GameApplication();

  ResourceManager::getInstance()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");

  app->setDefaultWorld(new GlobeOverviewWorld());

  //app->getDefaultGameSettings()->setScreenNum(1);
  //app->getDefaultGameSettings()->setFullscreenEnabled(true);

  GameState::AbstractGameStateList states;
  states.push_back(new GlobeOverviewState());
  states.push_back(new MainMenuState());

  return app->run(new LoadingGlobeOverviewState(states));

#endif
}
