#include "LoadingGlobeOverviewState.h"

#include "data/BoundariesData.h"
#include "nodes/GlobeModel.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/BackgroundModel.h"
#include "nodes/CountryNameOverlay.h"
#include "nodes/CountryOverlay.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "scripting/LuaControl.h"
#include "simulation/Simulation.h"
#include "simulation/ModelContainer.h"

#include <osgHelper/ioc/Injector.h>
#include <osgHelper/View.h>
#include <osgHelper/ppu/HDR.h>
#include <osgHelper/ppu/DOF.h>
#include <osgHelper/ppu/FXAA.h>

#include <utilsLib/Utils.h>

#include <QtOsgBridge/Helper.h>

#include <osg/GL2Extensions>

#include <luaHelper/IModManager.h>
#include <luaHelper/ILuaStateManager.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QResizeEvent>

#include <osgHelper/IResourceManager.h>

namespace onep
{
  struct LoadingGlobeOverviewState::Impl
  {
    Impl(osgHelper::ioc::Injector& injector)
      : fxaa(injector.inject<osgHelper::ppu::FXAA>())
      , hdr(injector.inject<osgHelper::ppu::HDR>())
      , dof(injector.inject<osgHelper::ppu::DOF>())
      , globeOverviewWorld(injector.inject<GlobeOverviewWorld>())
      , backgroundModel(injector.inject<BackgroundModel>())
      , globeModel(injector.inject<GlobeModel>())
      , countryNameOverlay(injector.inject<CountryNameOverlay>())
      , boundariesData(injector.inject<BoundariesData>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , resourceManager(injector.inject<osgHelper::IResourceManager>())
      , simulation(injector.inject<Simulation>())
      , lua(injector.inject<luaHelper::ILuaStateManager>())
      , model(injector.inject<ModelContainer>())
      , luaControl(injector.inject<LuaControl>())
      , modManager(injector.inject<luaHelper::IModManager>())
      , labelLoadingText(nullptr)
      , overlay(nullptr)
      , isFp64Supported(false)
    {

    }

    osg::ref_ptr<osgHelper::ppu::FXAA> fxaa;
    osg::ref_ptr<osgHelper::ppu::HDR>  hdr;
    osg::ref_ptr<osgHelper::ppu::DOF>  dof;

    osg::ref_ptr<GlobeOverviewWorld> globeOverviewWorld;
    osg::ref_ptr<BackgroundModel>    backgroundModel;
    osg::ref_ptr<GlobeModel>         globeModel;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;
    osg::ref_ptr<BoundariesData>     boundariesData;
    osg::ref_ptr<CountryOverlay>     countryOverlay;
    osg::ref_ptr<osgHelper::Camera>  camera;
    osg::ref_ptr<osgHelper::View>    view;

    osg::ref_ptr<osgHelper::IResourceManager> resourceManager;
    osg::ref_ptr<Simulation>                  simulation;
    osg::ref_ptr<luaHelper::ILuaStateManager>  lua;
    osg::ref_ptr<ModelContainer>              model;
    osg::ref_ptr<LuaControl>                  luaControl;

    osg::ref_ptr<luaHelper::IModManager> modManager;

    QPointer<QLabel>  labelLoadingText;
    QPointer<QWidget> overlay;

    bool isFp64Supported;

  };

  LoadingGlobeOverviewState::LoadingGlobeOverviewState(osgHelper::ioc::Injector& injector)
    : LoadingState(injector)
    , m(new Impl(injector))
  {
  }

  LoadingGlobeOverviewState::~LoadingGlobeOverviewState() = default;

  void LoadingGlobeOverviewState::onInitializeLoading(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data)
  {
    m->view   = mainWindow->getViewWidget()->getView();
    m->camera = m->view->getCamera(osgHelper::View::CameraType::Scene);

    const auto state = m->camera->getGraphicsContext()->getState();

    m->isFp64Supported = (state && osg::GL2Extensions::Get(state->getContextID(), false)->isGpuShaderFp64Supported);

    const auto projNear   = static_cast<float>(m->camera->getProjectionNear());
    const auto projFar    = static_cast<float>(m->camera->getProjectionFar());
    const auto resolution = m->view->getResolution();

    m->view->setClampColorEnabled(true);

    m->fxaa->setResolution(m->view->getResolution());
    m->dof->setZNear(projNear);
    m->dof->setZFar(projFar);

    m->view->addPostProcessingEffect(m->fxaa);
    m->view->addPostProcessingEffect(m->hdr);
    m->view->addPostProcessingEffect(m->dof, false);

    // initialize ui
    m->labelLoadingText = new QLabel(QString());
    m->labelLoadingText->setObjectName("LabelLoadingText");

    m->overlay = new QWidget();

    m->overlay->setGeometry(0, 0, int(resolution.x()), int(resolution.y()));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addStretch(1);
    layout->addWidget(m->labelLoadingText);

    m->overlay->setLayout(layout);

    mainWindow->getViewWidget()->addOverlayWidget(m->overlay);
  }

  void LoadingGlobeOverviewState::onUpdate(const SimulationData& data)
  {
    const auto dotCount = static_cast<int>(data.time * 10.0) % 4;
    auto loadingTextString = QObject::tr("Loading");

    for (int i = 0; i < dotCount; i++)
      loadingTextString += ".";

    m->labelLoadingText->setText(loadingTextString);
  }

  void LoadingGlobeOverviewState::onLoading()
  {
    m->modManager->loadModFromDirectory("./GameData/scripts");
    m->modManager->scanDirectoryForMods("./Mods");

    m->model->initializeState();

    // loading globe
    m->globeOverviewWorld->initialize();
    m->backgroundModel->loadStars("./GameData/data/stars.bin");

    m->globeModel->makeGlobeModel(m->isFp64Supported);

    m->boundariesData->loadBoundaries("./GameData/data/boundaries.dat");
    m->countryOverlay->loadCountries("./GameData/data/countries.dat", "./GameData/textures/earth/distance.png");

    m->countryNameOverlay->setEnabled(false);
    m->countryNameOverlay->initialize(m->countryOverlay->getCountryPresenters());

    // update neighbours data
    m->model->initializeCountryNeighbours(m->countryOverlay->getNeighbourships());

    m->luaControl->triggerLuaCallback(utilsLib::underlying(LuaDefines::Callback::ON_INITIALIZE));

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

  void LoadingGlobeOverviewState::onExitLoading()
  {
    QtOsgBridge::Helper::deleteWidget(m->overlay);

    m->camera->addCameraAlignedQuad(m->globeModel->getScatteringQuad());
    m->view->getRootGroup()->addChild(m->globeOverviewWorld);
  }

  void LoadingGlobeOverviewState::onRequestNewStates()
  {
    requestNewEventState<GlobeOverviewState>();
    requestNewEventState<MainMenuState>(NewEventStateMode::ExitCurrent);
  }

  void LoadingGlobeOverviewState::onResizeEvent(QResizeEvent* event)
  {
    m->overlay->setGeometry(0, 0, static_cast<int>(event->size().width()), static_cast<int>(event->size().height()));
  }
}
