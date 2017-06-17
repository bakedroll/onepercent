#include "OnePercentApplication.h"

#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "states/LoadingGlobeOverviewState.h"
#include "osgGaming/ResourceManager.h"
#include "osgGaming/PropertiesManager.h"

namespace onep
{
  OnePercentApplication::OnePercentApplication(int argc, char** argv)
    : QtGameApplication(argc, argv)
  {
    osgGaming::ResourceManager::getInstance()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");
    osgGaming::PropertiesManager::getInstance()->loadPropertiesFromXmlResource("./GameData/data/game_parameters.xml");
    osgGaming::PropertiesManager::getInstance()->loadPropertiesFromXmlResource("./GameData/data/localization/en.xml");
  }

  int OnePercentApplication::run()
  {
    osgGaming::GameState::AbstractGameStateList states;
    states.push_back(new GlobeOverviewState());
    states.push_back(new MainMenuState());

    return QtGameApplication::run(new LoadingGlobeOverviewState(states));
  }
}
