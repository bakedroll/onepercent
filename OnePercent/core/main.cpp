#include <osgGaming/ResourceManager.h>

#include "core/QtGameApplication.h"

#include "states/LoadingGlobeOverviewState.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"

#include "nodes/GlobeOverviewWorld.h"

#include "widgets/MainWindow.h"

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


  ref_ptr<QtGameApplication> app = new QtGameApplication(argc, argv);
  //ref_ptr<GameApplication> app = new GameApplication();

	app->setDefaultWorld(new GlobeOverviewWorld());

	//app->getDefaultGameSettings()->setScreenNum(1);
	//app->getDefaultGameSettings()->setFullscreenEnabled(true);

	GameState::AbstractGameStateList states;
	states.push_back(new GlobeOverviewState());
	states.push_back(new MainMenuState());

  MainWindow mainwindow(app->getViewer());
  mainwindow.show();

	return app->run(new LoadingGlobeOverviewState(states));
}
