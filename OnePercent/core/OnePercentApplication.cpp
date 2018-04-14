#include "OnePercentApplication.h"

#include "scripting/LuaStateManager.h"

#include "core/Observables.h"
#include "nodes/CountryOverlay.h"
#include "nodes/BoundariesMesh.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "states/LoadingGlobeOverviewState.h"
#include "states/GlobeInteractionState.h"
#include "scripting/LuaLogger.h"
#include "simulation/SkillsContainer.h"
#include "simulation/CountriesContainer.h"
#include "simulation/SimulationStateContainer.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/PropertiesManager.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/HighDynamicRangeEffect.h>

#include <QFile>

namespace onep
{
  OnePercentApplication::OnePercentApplication(int& argc, char** argv)
    : QtGameApplication(argc, argv)
  {
  }

  int OnePercentApplication::run()
  {
    return QtGameApplication::run<LoadingGlobeOverviewState>();
  }

  void OnePercentApplication::registerComponents(osgGaming::InjectionContainer& container)
  {
    registerEssentialComponents();

    // States
    container.registerType<LoadingGlobeOverviewState>();
    container.registerType<GlobeOverviewState>();
    container.registerType<MainMenuState>();
    container.registerType<GlobeInteractionState>();

    // osg nodes
    container.registerSingletonType<GlobeOverviewWorld>();
    container.registerSingletonType<BackgroundModel>();
    container.registerSingletonType<GlobeModel>();
    container.registerSingletonType<CountryNameOverlay>();
    container.registerSingletonType<BoundariesMesh>();
    container.registerSingletonType<CountryOverlay>();

    // simulation
    container.registerSingletonType<Simulation>();
    container.registerSingletonType<SkillsContainer>();
    container.registerSingletonType<CountriesContainer>();
    container.registerSingletonType<SimulationStateContainer>();

    // effects
    container.registerType<osgGaming::FastApproximateAntiAliasingEffect>();
    container.registerType<osgGaming::HighDynamicRangeEffect>();
    container.registerType<osgGaming::DepthOfFieldEffect>();

    // wrapper
    container.registerSingletonType<LuaStateManager>();
    container.registerSingletonType<LuaLogger>();

    // Observables
    container.registerSingletonType<ONumSkillPoints>();
    container.registerSingletonType<ODay>();
  }

  void OnePercentApplication::initialize(osgGaming::Injector& injector)
  {
    injector.inject<osgGaming::ResourceManager>()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");
    injector.inject<osgGaming::PropertiesManager>()->loadPropertiesFromXmlResource("./GameData/data/game_parameters.xml");

    // initialize Lua classes
    osg::ref_ptr<LuaStateManager> lua = injector.inject<LuaStateManager>();
    lua->registerClassInstance<Simulation>(injector.inject<Simulation>());
    lua->registerClassInstance<LuaLogger>(injector.inject<LuaLogger>());

    setDefaultWorld(injector.inject<GlobeOverviewWorld>());

    // load CSS
    QFile file("./GameData/CSS/style.css");
    if (!file.open(QIODevice::ReadOnly))
    {
      OSGG_LOG_FATAL("Could not load style sheets");
      assert(false);
    }
    else
    {
      qApplication()->setStyleSheet(QString(file.readAll()));
      file.close();
    }
  }
}
