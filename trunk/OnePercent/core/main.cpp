#include <osgGaming/ResourceManager.h>
#include <osgGaming/NativeGameApplication.h>
#include <osgGaming/View.h>

#include "core/OnePercentApplication.h"

#include "states/LoadingGlobeOverviewState.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"

#include "nodes/GlobeOverviewWorld.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

// #define INITIALIZE_QT

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

  ref_ptr<NativeGameApplication> app = new NativeGameApplication();

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
