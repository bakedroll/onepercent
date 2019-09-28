#include "LoadingGlobeOverviewState.h"

#include "nodes/GlobeModel.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/BackgroundModel.h"
#include "nodes/BoundariesMesh.h"
#include "nodes/CountryNameOverlay.h"
#include "nodes/CountryOverlay.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "scripting/LuaStateManager.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaControl.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"
#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"

#include <osgDB/ReadFile>

#include <osgGaming/Injector.h>
#include <osgGaming/View.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

#include <QVBoxLayout>
#include <QLabel>

#include <osgGaming/ResourceManager.h>

namespace onep
{
  struct LoadingGlobeOverviewState::Impl
  {
    Impl(osgGaming::Injector& injector)
      : fxaa(injector.inject<osgGaming::FastApproximateAntiAliasingEffect>())
      , hdr(injector.inject<osgGaming::HighDynamicRangeEffect>())
      , dof(injector.inject<osgGaming::DepthOfFieldEffect>())
      , globeOverviewWorld(injector.inject<GlobeOverviewWorld>())
      , backgroundModel(injector.inject<BackgroundModel>())
      , globeModel(injector.inject<GlobeModel>())
      , countryNameOverlay(injector.inject<CountryNameOverlay>())
      , boundariesMesh(injector.inject<BoundariesMesh>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , resourceManager(injector.inject<osgGaming::ResourceManager>())
      , simulation(injector.inject<Simulation>())
      , lua(injector.inject<LuaStateManager>())
      , model(injector.inject<ModelContainer>())
      , luaControl(injector.inject<LuaControl>())
      , labelLoadingText(nullptr)
      , overlay(nullptr)
    {

    }

    osg::ref_ptr<osgGaming::FastApproximateAntiAliasingEffect> fxaa;
    osg::ref_ptr<osgGaming::HighDynamicRangeEffect> hdr;
    osg::ref_ptr<osgGaming::DepthOfFieldEffect> dof;

    osg::ref_ptr<GlobeOverviewWorld> globeOverviewWorld;
    osg::ref_ptr<BackgroundModel> backgroundModel;
    osg::ref_ptr<GlobeModel> globeModel;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;
    osg::ref_ptr<BoundariesMesh> boundariesMesh;
    osg::ref_ptr<CountryOverlay> countryOverlay;

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<Simulation> simulation;
    osg::ref_ptr<LuaStateManager> lua;
    osg::ref_ptr<ModelContainer> model;
    osg::ref_ptr<LuaControl> luaControl;

    QLabel* labelLoadingText;
    VirtualOverlay* overlay;
  };

  LoadingGlobeOverviewState::LoadingGlobeOverviewState(osgGaming::Injector& injector)
    : QtGameLoadingState(injector)
    , m(new Impl(injector))
  {
  }

  LoadingGlobeOverviewState::~LoadingGlobeOverviewState()
  {
  }

  void LoadingGlobeOverviewState::initialize()
  {
    float projNear = float(getWorld(getView(0))->getCameraManipulator()->getProjectionNear());
    float projFar = float(getWorld(getView(0))->getCameraManipulator()->getProjectionFar());

    getView(0)->setClampColorEnabled(true);

    m->fxaa->setResolution(getView(0)->getResolution());
    m->dof->setZNear(projNear);
    m->dof->setZFar(projFar);

    getView(0)->addPostProcessingEffect(m->fxaa);
    getView(0)->addPostProcessingEffect(m->hdr);
    getView(0)->addPostProcessingEffect(m->dof, false);

    osg::Vec2f resolution = getView(0)->getResolution();

    // initialize ui
    m->labelLoadingText = new QLabel(QString());
    m->labelLoadingText->setObjectName("LabelLoadingText");

    m->overlay = new VirtualOverlay();

    m->overlay->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addStretch(1);
    layout->addWidget(m->labelLoadingText);

    m->overlay->setLayout(layout);

    getOverlayCompositor()->addVirtualOverlay(m->overlay);
  }

  osgGaming::GameState::StateEvent* LoadingGlobeOverviewState::update()
  {
    int dotCount = int(getSimulationTime() * 10.0) % 4;
    QString loadingTextString = QObject::tr("Loading");

    for (int i = 0; i < dotCount; i++)
      loadingTextString += ".";

    m->labelLoadingText->setText(loadingTextString);

    return stateEvent_default();
  }

  void LoadingGlobeOverviewState::load(osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::Hud> hud, osg::ref_ptr<osgGaming::GameSettings> settings)
  {
    // Loading scripts
    m->lua->loadScript(":/Resources/scripts/core.lua");
    m->lua->loadScript(":/Resources/scripts/gameplay.lua");
    m->lua->loadScript(":/Resources/scripts/helper.lua");
    m->lua->loadScript(":/Resources/scripts/defines.lua");

    m->lua->loadScript("./GameData/scripts/data/config.lua");

    m->lua->loadScript("./GameData/scripts/data/branches.lua");
    m->lua->loadScript("./GameData/scripts/data/skills.lua");
    m->lua->loadScript("./GameData/scripts/data/countries.lua");
    m->lua->loadScript("./GameData/scripts/data/values.lua");

    m->lua->loadScript("./GameData/scripts/control/skills.lua");
    m->lua->loadScript("./GameData/scripts/control/visuals.lua");
    m->lua->loadScript("./GameData/scripts/control/prototypes.lua");

    m->model->initializeState();

    // loading globe
    m->globeOverviewWorld->initialize();
    m->backgroundModel->loadStars("./GameData/data/stars.bin");

    m->globeModel->makeGlobeModel();
    world->getCameraManipulator()->addCameraAlignedQuad(m->globeModel->getScatteringQuad());

    m->boundariesMesh->loadBoundaries("./GameData/data/boundaries.dat");
    m->boundariesMesh->makeOverallBoundaries(0.005f);

    m->countryOverlay->loadCountries(
      "./GameData/data/countries.dat",
      "./GameData/textures/earth/distance.png",
      m->boundariesMesh->getCountryVertices(),
      m->boundariesMesh->getCountryTexcoords());

    m->countryNameOverlay->setEnabled(false);
    m->countryNameOverlay->initialize(m->countryOverlay->getCountryPresenters());

    // update neighbours data
    m->model->initializeCountryNeighbours(m->countryOverlay->getNeighbourships());

    m->luaControl->triggerLuaCallback(LuaDefines::Callback::ON_INITIALIZE);

    // loading simulation
    m->simulation->prepare();

    m->resourceManager->clearCache();


    /** 
     * CODE TO GENERATE countries.lua
     *
    QFile file("./GameData/countries.lua.gen");
    assert_return(file.open(QIODevice::WriteOnly));

    QTextStream stream(&file);

    stream << "local countries = {\n";

    LuaCountry::Map& countries = m->countriesContainer->getCountryMap();
    for (LuaCountry::Map::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      stream << "  {\n";
      stream << QString("    id = %1,\n").arg(it->second->getId());
      stream << QString("    name = \"%1\",\n").arg(QString::fromLocal8Bit(it->second->getName().c_str()));
      stream << QString("    init_values = {\n");
      stream << QString("      [\"population\"] =  %1,\n").arg(it->second->getPopulation());
      stream << QString("      [\"wealth\"] =  %1\n").arg(int(it->second->getWealth()));
      stream << QString("    }\n");
      stream << "  }";

      if (it->first != countries.rbegin()->first)
        stream << ",";

      stream << "\n";
    }

    stream << "}\n\n";
    stream << "control.create_countries(countries)\n";

    file.close();
    */
  }

  void LoadingGlobeOverviewState::onResizeEvent(float width, float height)
  {
    m->overlay->setGeometry(0, 0, int(width), int(height));
  }

  void LoadingGlobeOverviewState::injectNextStates(osgGaming::Injector& injector, AbstractGameStateList& states)
  {
    states.push_back(injector.inject<GlobeOverviewState>());
    states.push_back(injector.inject<MainMenuState>());
  }
}
