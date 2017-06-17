#include "LoadingGlobeOverviewState.h"

#include "nodes/GlobeModel.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/BackgroundModel.h"
#include "widgets/VirtualOverlay.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/NativeView.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

#include <QVBoxLayout>
#include <QLabel>

using namespace osg;
using namespace osgGaming;
using namespace osgText;
using namespace std;

namespace onep
{
  struct LoadingGlobeOverviewState::Impl
  {
    Impl()
      : labelLoadingText(nullptr)
    {

    }

    QLabel* labelLoadingText;
  };

  LoadingGlobeOverviewState::LoadingGlobeOverviewState(ref_ptr<GameState> nextState)
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

    getView(0)->addPostProcessingEffect(new FastApproximateAntiAliasingEffect(getView(0)->getResolution()));
    getView(0)->addPostProcessingEffect(new HighDynamicRangeEffect());
    getView(0)->addPostProcessingEffect(new DepthOfFieldEffect(projNear, projFar), false);
  }

  VirtualOverlay* LoadingGlobeOverviewState::createVirtualOverlay()
  {
    m->labelLoadingText = new QLabel(QString());
    m->labelLoadingText->setStyleSheet("font-size: 20pt; margin: 10px;");

    VirtualOverlay* overlay = new VirtualOverlay();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addStretch(1);
    layout->addWidget(m->labelLoadingText);

    overlay->setLayout(layout);

    return overlay;
  }

  GameState::StateEvent* LoadingGlobeOverviewState::update()
  {
    int dotCount = int(getSimulationTime() * 10.0) % 4;
    QString loadingTextString = QObject::tr("Loading");

    for (int i = 0; i < dotCount; i++)
      loadingTextString += ".";

    m->labelLoadingText->setText(loadingTextString);

    return stateEvent_default();
  }

  void LoadingGlobeOverviewState::load(ref_ptr<World> world, osg::ref_ptr<Hud> hud, ref_ptr<GameSettings> settings)
  {
    ref_ptr<GlobeOverviewWorld> globeWorld = static_cast<GlobeOverviewWorld*>(world.get());

    ref_ptr<GlobeModel> globe = new GlobeModel(world->getCameraManipulator());


    ref_ptr<BackgroundModel> backgroundModel = new BackgroundModel();

    globeWorld->getSimulation()->setGlobeModel(globe);
    globeWorld->getSimulation()->loadCountries("./GameData/data/countries.dat");
    globeWorld->getSimulation()->loadSkillsXml("./GameData/data/skills/passive.xml");

    ref_ptr<CountryOverlay> countryOverlay = new CountryOverlay();
    countryOverlay->setEnabled(false);
    countryOverlay->setCountryMap(globe->getCountryMeshs());

    globeWorld->setGlobeModel(globe);
    globeWorld->setCountryOverlay(countryOverlay);
    globeWorld->setBackgroundModel(backgroundModel);
  }

}
