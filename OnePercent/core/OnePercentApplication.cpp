#include "OnePercentApplication.h"

#include "scripting/LuaStateManager.h"

#include "core/Macros.h"
#include "core/Observables.h"
#include "data/BoundariesData.h"
#include "nodes/CountryNameOverlay.h"
#include "nodes/CountryOverlay.h"
#include "nodes/BackgroundModel.h"
#include "nodes/BoundariesMesh.h"
#include "nodes/CountryPresenter.h"
#include "nodes/TextNode.h"
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
#include "scripting/LuaPropertyDefinitions.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"

#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/HighDynamicRangeEffect.h>

#include <QDir>

namespace onep
{
  void registerLuaClasses(osgGaming::Injector& injector)
  {
    osg::ref_ptr<LuaStateManager> lua = injector.inject<LuaStateManager>();

    lua->registerDefinition<LuaExternalClassDefinitions>();

    lua->registerDefinition<LuaCallbackRegistry::Definition>();
    lua->registerDefinition<LuaVisualOsgNode<osg::Group>::Definition>();
    lua->registerDefinition<LuaVisualOsgNode<osg::Geode>::Definition>();
    lua->registerDefinition<LuaVisualOsgNode<osg::MatrixTransform>::Definition>();
    lua->registerDefinition<LuaVisualOsgNode<osg::PositionAttitudeTransform>::Definition>();

    lua->registerDefinition<LuaSkill::Definition>();
    lua->registerDefinition<CountryOverlay::Definition>();
    lua->registerDefinition<CountryNode::Definition>();
    lua->registerDefinition<CountryPresenter::Definition>();
    lua->registerDefinition<TextNode::Definition>();

    lua->registerDefinition<LuaConfig::Definition>();
    lua->registerDefinition<LuaControl::Definition>();
    lua->registerDefinition<LuaSimulation::Definition>();
    lua->registerDefinition<LuaVisuals::Definition>();
    lua->registerDefinition<LuaLogger::Definition>();

    lua->registerDefinition<LuaPropertyDefinitions>(injector);
  }

  osg::ref_ptr<osgText::Font> loadFontFile(const std::string& filename)
  {
    QFile fontFile(QString::fromStdString(filename));
    if (!fontFile.open(QIODevice::ReadOnly))
    {
      return nullptr;
    }

    const auto size = fontFile.size();
    auto fontData = fontFile.readAll();

    const auto byteArray = new char[size];
    memcpy(byteArray, fontData.data(), size);

    const std::string str(byteArray, size);

    std::stringstream stream;
    stream << str;

    fontFile.close();

    const auto extension = osgDB::getLowerCaseFileExtension(filename);
    const osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension(extension);
    auto obj = rw->readObject(stream);

    delete [] byteArray;

    return dynamic_cast<osgText::Font*>(obj.getObject());
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
    return QtGameApplication::runGame<LoadingGlobeOverviewState>();
  }

  void OnePercentApplication::registerComponents(osgGaming::InjectionContainer& container)
  {
    registerEssentialComponents();

    // States
    container.registerType<LoadingGlobeOverviewState>();
    container.registerType<GlobeOverviewState>();
    container.registerType<MainMenuState>();
    container.registerType<GlobeInteractionState>();

    // data
    container.registerSingletonType<BoundariesData>();

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
    std::string fontFilename = ":/Resources/fonts/coolvetica rg.ttf";
    auto font = loadFontFile(fontFilename);
    if (font)
    {
      osgGaming::ResourceManager::setDefaultFont(font);
    }
    else
    {
      OSGG_QLOG_WARN(QString("Could not load font file '%1'").arg(fontFilename.c_str()));
    }

    registerLuaClasses(injector);

    m->simulation = injector.inject<Simulation>();

    setDefaultWorld(injector.inject<GlobeOverviewWorld>());

    OSGG_LOG_INFO("Loading stylesheets");
    m->loadStylesheets(":/Resources/stylesheets/");
  }

  void OnePercentApplication::deinitialize()
  {
    m->simulation->shutdownUpdateThread();
    m->simulation.release();

    QtGameApplication::deinitialize();
  }
}
