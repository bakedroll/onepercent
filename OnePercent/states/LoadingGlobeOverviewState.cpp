#include "LoadingGlobeOverviewState.h"

#include "nodes/GlobeModel.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/BackgroundModel.h"
#include "widgets/OverlayCompositor.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/NativeView.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

#include <QVBoxLayout>
#include <QLabel>

namespace onep
{
  struct LoadingGlobeOverviewState::Impl
  {
    Impl()
      : labelLoadingText(nullptr)
      , overlay(nullptr)
    {

    }

    QLabel* labelLoadingText;
    VirtualOverlay* overlay;
  };

  LoadingGlobeOverviewState::LoadingGlobeOverviewState(osg::ref_ptr<osgGaming:: GameState> nextState)
    : QtGameLoadingState(nextState)
    , m(new Impl())
  {
  }

  LoadingGlobeOverviewState::LoadingGlobeOverviewState(AbstractGameState::AbstractGameStateList nextStates)
    : QtGameLoadingState(nextStates)
    , m(new Impl())
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

    getView(0)->addPostProcessingEffect(new osgGaming::FastApproximateAntiAliasingEffect(getView(0)->getResolution()));
    getView(0)->addPostProcessingEffect(new osgGaming::HighDynamicRangeEffect());
    getView(0)->addPostProcessingEffect(new osgGaming::DepthOfFieldEffect(projNear, projFar), false);

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
    osg::ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

    osg::ref_ptr<GlobeModel> globe = new GlobeModel(world->getCameraManipulator());


    osg::ref_ptr<BackgroundModel> backgroundModel = new BackgroundModel();

    globeWorld->getSimulation()->setGlobeModel(globe);
    globeWorld->getSimulation()->loadCountries("./GameData/data/countries.dat");
    globeWorld->getSimulation()->loadSkillsXml("./GameData/data/skills/passive.xml");

    osg::ref_ptr<CountryOverlay> countryOverlay = new CountryOverlay();
    countryOverlay->setEnabled(false);
    countryOverlay->setCountryMap(globe->getCountryMeshs());

    globeWorld->setGlobeModel(globe);
    globeWorld->setCountryOverlay(countryOverlay);
    globeWorld->setBackgroundModel(backgroundModel);
  }

  void LoadingGlobeOverviewState::onResizeEvent(float width, float height)
  {
    m->overlay->setGeometry(0, 0, int(width), int(height));
  }
}
