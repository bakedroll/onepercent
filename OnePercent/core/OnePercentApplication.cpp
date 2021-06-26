#include "OnePercentApplication.h"

#include "scripting/LuaStateManager.h"

#include "core/ModManager.h"
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

#include <osgHelper/ResourceManager.h>
#include <osgHelper/ppu/FXAA.h>
#include <osgHelper/ppu/DOF.h>
#include <osgHelper/ppu/HDR.h>

#include <QtOsgBridge/Macros.h>

#include <QDir>

namespace onep
{
  void registerLuaClasses(osgHelper::ioc::Injector& injector)
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

  OnePercentApplication::OnePercentApplication(int& argc, char** argv)
    : QtGameApplication(argc, argv)
  {
  }

  int OnePercentApplication::run()
  {
    return runGame<LoadingGlobeOverviewState>();
  }

  void OnePercentApplication::registerComponents(osgHelper::ioc::InjectionContainer& container)
  {
    registerEssentialComponents();

    // Core
    container.registerSingletonType<ModManager>();

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
    container.registerType<osgHelper::ppu::FXAA>();
    container.registerType<osgHelper::ppu::HDR>();
    container.registerType<osgHelper::ppu::DOF>();

    // Observables
    container.registerSingletonType<ONumSkillPoints>();
    container.registerSingletonType<ODay>();
  }

  void OnePercentApplication::initialize(osgHelper::ioc::Injector& injector)
  {
    const auto fontFilename = ":/Resources/fonts/coolvetica rg.ttf";
    const auto font = loadFontFile(fontFilename);
    if (font)
    {
      osgHelper::ResourceManager::setDefaultFont(font);
    }
    else
    {
      OSGH_QLOG_WARN(QString("Could not load font file '%1'").arg(fontFilename));
    }

    registerLuaClasses(injector);

    m_simulation = injector.inject<Simulation>();

    OSGH_LOG_INFO("Loading stylesheets");
    loadStylesheets(":/Resources/stylesheets/");
  }

  void OnePercentApplication::deinitialize()
  {
    m_simulation->shutdownUpdateThread();
    m_simulation.release();

    QtGameApplication::deinitialize();
  }

  void OnePercentApplication::loadStylesheets(const QString& path)
  {
      QDir dir(path);
      if (!dir.exists())
      {
        OSGH_QLOG_WARN(QString("Could not find stylesheet directory: %1").arg(path));
        return;
      }

      QStringList cssFiles = dir.entryList(QDir::Files | QDir::NoSymLinks);

      QString stylesheetStr;
      for (QStringList::iterator it = cssFiles.begin(); it != cssFiles.end(); ++it)
      {
        QFile file(dir.filePath(*it));
        if (!file.open(QIODevice::ReadOnly))
        {
          OSGH_QLOG_WARN(QString("Could not read stylesheet file: %1").arg(file.fileName()));
          continue;
        }

        stylesheetStr.append(QString(file.readAll()) + "\n");
        file.close();
      }

      setStyleSheet(stylesheetStr);
  }
}
