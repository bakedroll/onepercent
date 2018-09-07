#include "OnePercentApplication.h"

#include "scripting/LuaStateManager.h"

#include "core/Macros.h"
#include "core/Observables.h"
#include "nodes/CountryNameOverlay.h"
#include "nodes/CountryOverlay.h"
#include "nodes/BackgroundModel.h"
#include "nodes/BoundariesMesh.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "states/LoadingGlobeOverviewState.h"
#include "states/GlobeInteractionState.h"
#include "scripting/ConfigManager.h"
#include "scripting/LuaLogger.h"
#include "scripting/Visuals.h"
#include "simulation/Simulation.h"
#include "simulation/SkillsContainer.h"
#include "simulation/CountriesContainer.h"
#include "simulation/SimulationStateContainer.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/HighDynamicRangeEffect.h>

#include <QFile>
#include <QDir>

namespace onep
{
  struct OnePercentApplication::Impl
  {
    Impl(OnePercentApplication* b) : base(b) {}

    OnePercentApplication* base;
    Simulation::Ptr simulation;

    void loadStylesheets(const QString& path)
    {
      QDir dir(path);
      if (!dir.exists())
      {
        OSGG_QLOG_WARN(QString("Could not find stylesheet directory: %1").arg(path));
        return;
      }

      QStringList cssFiles = dir.entryList(QDir::Files | QDir::NoSymLinks);

      QString stylesheetStr;
      for (QStringList::iterator it = cssFiles.begin(); it != cssFiles.end(); ++it)
      {
        QFile file(dir.filePath(*it));
        if (!file.open(QIODevice::ReadOnly))
        {
          OSGG_QLOG_WARN(QString("Could not read stylesheet file: %1").arg(file.fileName()));
          continue;
        }

        stylesheetStr.append(QString(file.readAll()) + "\n");
        file.close();
      }

      base->setStyleSheet(stylesheetStr);
    }
  };

  OnePercentApplication::OnePercentApplication(int& argc, char** argv)
    : QtGameApplication(argc, argv)
    , m(new Impl(this))
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

    // scripting
    container.registerSingletonType<Visuals>();

    // effects
    container.registerType<osgGaming::FastApproximateAntiAliasingEffect>();
    container.registerType<osgGaming::HighDynamicRangeEffect>();
    container.registerType<osgGaming::DepthOfFieldEffect>();

    // wrapper
    container.registerSingletonType<LuaStateManager>();
    container.registerSingletonType<LuaLogger>();
    container.registerSingletonType<ConfigManager>();

    // Observables
    container.registerSingletonType<ONumSkillPoints>();
    container.registerSingletonType<ODay>();
  }

  void OnePercentApplication::initialize(osgGaming::Injector& injector)
  {
    OSGG_LOG_INFO("Loading fonts");
    injector.inject<osgGaming::ResourceManager>()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");

    // initialize Lua classes
    osg::ref_ptr<LuaStateManager> lua = injector.inject<LuaStateManager>();
    lua->registerClassInstance<Simulation>(injector.inject<Simulation>());
    lua->registerClassInstance<Visuals>(injector.inject<Visuals>());
    lua->registerClassInstance<LuaLogger>(injector.inject<LuaLogger>());

    m->simulation = injector.inject<Simulation>();

    setDefaultWorld(injector.inject<GlobeOverviewWorld>());

    OSGG_LOG_INFO("Loading stylesheets");
    m->loadStylesheets("./GameData/CSS/");
  }

  void OnePercentApplication::deinitialize()
  {
    m->simulation->shutdownUpdateThread();
    m->simulation.release();

    QtGameApplication::deinitialize();
  }
}
