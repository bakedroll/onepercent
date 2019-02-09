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
#include "scripting/LuaExternalClassDefinitions.h"
#include "scripting/LuaConfig.h"
#include "scripting/LuaLogger.h"
#include "scripting/LuaControl.h"
#include "scripting/LuaVisuals.h"
#include "scripting/LuaSimulation.h"
#include "scripting/LuaSkill.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/HighDynamicRangeEffect.h>

#include <QFile>
#include <QDir>

namespace onep
{
  void registerLuaClasses(osgGaming::Injector& injector)
  {
    osg::ref_ptr<LuaStateManager> lua = injector.inject<LuaStateManager>();

    lua->registerClass<LuaExternalClassDefinitions>();

    lua->registerClass<LuaCallbackRegistry::Definition>();
    lua->registerClass<LuaVisualOsgNode<osg::Geode>::Definition>();

    lua->registerClass<LuaSkill::Definition>();
    lua->registerClass<CountryOverlay::Definition>();
    lua->registerClass<CountryNode::Definition>();

    lua->registerClass<LuaConfig::Definition>();
    lua->registerClass<LuaControl::Definition>();
    lua->registerClass<LuaSimulation::Definition>();
    lua->registerClass<LuaVisuals::Definition>();
    lua->registerClass<LuaLogger::Definition>();

    lua->makeGlobalInstance("config", injector.inject<LuaConfig>().get());
    lua->makeGlobalInstance("control", injector.inject<LuaControl>().get());
    lua->makeGlobalInstance("simulation", injector.inject<LuaSimulation>().get());
    lua->makeGlobalInstance("visuals", injector.inject<LuaVisuals>().get());
    lua->makeGlobalInstance("log", injector.inject<LuaLogger>().get());
    lua->makeGlobalInstance("countries", injector.inject<CountryOverlay>().get());
  }

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
    container.registerSingletonType<ModelContainer>();

    // scripting
    container.registerSingletonType<LuaControl>();
    container.registerSingletonType<LuaVisuals>();
    container.registerSingletonType<LuaStateManager>();
    container.registerSingletonType<LuaLogger>();
    container.registerSingletonType<LuaConfig>();
    container.registerSingletonType<LuaSimulation>();

    // effects
    container.registerType<osgGaming::FastApproximateAntiAliasingEffect>();
    container.registerType<osgGaming::HighDynamicRangeEffect>();
    container.registerType<osgGaming::DepthOfFieldEffect>();

    // Observables
    container.registerSingletonType<ONumSkillPoints>();
    container.registerSingletonType<ODay>();
  }

  void OnePercentApplication::initialize(osgGaming::Injector& injector)
  {
    OSGG_LOG_INFO("Loading fonts");
    injector.inject<osgGaming::ResourceManager>()->setDefaultFontResourceKey("./GameData/fonts/coolvetica rg.ttf");

    registerLuaClasses(injector);

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
