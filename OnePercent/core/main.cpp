#include <osgGaming/ResourceManager.h>

#include "core/OnePercentApplication.h"

using namespace osgGaming;
using namespace onep;
using namespace osg;

#ifdef WIN32
#include "core/CrashDumpWriter.h"
#endif

#include <QDir>

#define INITIALIZE_QT

int main(int argc, char** argv)
{
	/*ref_ptr<PackageResourceLoader> resourceLoader = new PackageResourceLoader();
	resourceLoader->registerPackage("./GameData/Data.pak");
	resourceLoader->registerPackage("./GameData/Shader.pak");

	ResourceManager::getInstance()->setResourceLoader(resourceLoader);*/


#ifdef INITIALIZE_QT

#ifdef WIN32
  SetUnhandledExceptionFilter(unhandled_handler);
#endif

  ref_ptr<OnePercentApplication> app = new OnePercentApplication(argc, argv);
  return app->run();

#else

  ref_ptr<NativeGameApplication> app = new NativeGameApplication();
  //app->getViewer()->addView(new osgGaming::NativeView());

  ResourceManager::getInstance()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");
  osgGaming::PropertiesManager::getInstance()->loadPropertiesFromXmlResource("./GameData/data/game_parameters.xml");
  osgGaming::PropertiesManager::getInstance()->loadPropertiesFromXmlResource("./GameData/data/localization/en.xml");

  app->setDefaultWorld(new GlobeOverviewWorld());

  //app->getDefaultGameSettings()->setScreenNum(1);
  //app->getDefaultGameSettings()->setFullscreenEnabled(true);

  GameState::AbstractGameStateList states;
  states.push_back(new GlobeOverviewState());
  states.push_back(new MainMenuState());

  return app->run(new LoadingGlobeOverviewState(states));

#endif
}
