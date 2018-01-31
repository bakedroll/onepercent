#include "LoadingGlobeOverviewState.h"

#include "nodes/GlobeModel.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/BackgroundModel.h"
#include "states/GlobeOverviewState.h"
#include "states/MainMenuState.h"
#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/Injector.h>
#include <osgGaming/NativeView.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

#include <QVBoxLayout>
#include <QLabel>
#include <chrono>

namespace onep
{
  struct LoadingGlobeOverviewState::Impl
  {
    Impl(osgGaming::Injector& injector)
      : fxaa(injector.inject<osgGaming::FastApproximateAntiAliasingEffect>())
      , hdr(injector.inject<osgGaming::HighDynamicRangeEffect>())
      , dof(injector.inject<osgGaming::DepthOfFieldEffect>())
      , backgroundModel(injector.inject<BackgroundModel>())
      , globeModel(injector.inject<GlobeModel>())
      , countryNameOverlay(injector.inject<CountryNameOverlay>())
      , labelLoadingText(nullptr)
      , overlay(nullptr)
    {

    }

    osg::ref_ptr<osgGaming::FastApproximateAntiAliasingEffect> fxaa;
    osg::ref_ptr<osgGaming::HighDynamicRangeEffect> hdr;
    osg::ref_ptr<osgGaming::DepthOfFieldEffect> dof;

    osg::ref_ptr<BackgroundModel> backgroundModel;
    osg::ref_ptr<GlobeModel> globeModel;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;

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
    m->backgroundModel->loadStars("./GameData/data/stars.bin");

    osg::ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

    m->globeModel->loadFromDisk(world->getCameraManipulator());
    m->globeModel->getBoundariesMesh()->loadBoundaries("./GameData/data/boundaries.dat");
    m->globeModel->getBoundariesMesh()->makeOverallBoundaries(0.005f);

    m->globeModel->getCountryOverlay()->loadCountries(
      "./GameData/data/countries.dat",
      "./GameData/textures/earth/distance.png",
      m->globeModel->getBoundariesMesh()->getCountryVertices(),
      m->globeModel->getBoundariesMesh()->getCountryTexcoords());

    globeWorld->getSimulation()->loadSkillsXml("./GameData/data/skills/passive.xml");
    globeWorld->getSimulation()->attachCountries(m->globeModel->getCountryOverlay()->getCountryMeshs());

    m->countryNameOverlay->setEnabled(false);
    m->countryNameOverlay->setCountryMap(m->globeModel->getCountryOverlay()->getCountryMeshs());

    globeWorld->setGlobeModel(m->globeModel);
    globeWorld->setCountryNameOverlay(m->countryNameOverlay);
    globeWorld->setBackgroundModel(m->backgroundModel);
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
