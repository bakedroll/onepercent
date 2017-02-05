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

#define INITIALIZE_QT

int main(int argc, char** argv)
{
  // Observer test
  osgGaming::Observable<int> observable(5);

  osgGaming::Observer<int>::Ptr obs1 = observable.connect([](int value)
  {
    printf("Obs1 value: %d\n", value);
  });

  osgGaming::Observer<int>::Ptr obs2 = observable.connect([](int value)
  {
    printf("Obs2 value: %d\n", value);
  });

  observable.set(2);

  obs1.release();

  observable.set(3);


	/*ref_ptr<PackageResourceLoader> resourceLoader = new PackageResourceLoader();
	resourceLoader->registerPackage("./GameData/Data.pak");
	resourceLoader->registerPackage("./GameData/Shader.pak");

	ResourceManager::getInstance()->setResourceLoader(resourceLoader);*/
	ResourceManager::getInstance()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");


#ifdef INITIALIZE_QT
  ref_ptr<QtGameApplication> app = new QtGameApplication(argc, argv);
#else
  ref_ptr<GameApplication> app = new GameApplication();
#endif

	app->setDefaultWorld(new GlobeOverviewWorld());

	//app->getDefaultGameSettings()->setScreenNum(1);
	//app->getDefaultGameSettings()->setFullscreenEnabled(true);

	GameState::AbstractGameStateList states;
	states.push_back(new GlobeOverviewState());
	states.push_back(new MainMenuState());

#ifdef INITIALIZE_QT
  MainWindow mainwindow(app->getViewer());
  mainwindow.show();
  app->setInputManager(mainwindow.getViewWidget());
#endif

	return app->run(new LoadingGlobeOverviewState(states));
}
