#include "LoadingGlobeOverviewState.h"

#include "nodes/GlobeModel.h"
#include "nodes/GlobeOverviewWorld.h"
#include "nodes/BackgroundModel.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/PropertiesManager.h>
#include <osgGaming/NativeView.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

using namespace osg;
using namespace osgGaming;
using namespace osgText;
using namespace std;

namespace onep
{
  LoadingGlobeOverviewState::LoadingGlobeOverviewState(ref_ptr<GameState> nextState)
    : GameLoadingState(nextState)
  {

  }

  LoadingGlobeOverviewState::LoadingGlobeOverviewState(AbstractGameState::AbstractGameStateList nextStates)
    : GameLoadingState(nextStates)
  {

  }

  void LoadingGlobeOverviewState::initialize()
  {
    PropertiesManager::getInstance()->loadPropertiesFromXmlResource("./GameData/data/localization/en.xml");

    float projNear = float(getWorld()->getCameraManipulator()->getProjectionNear());
    float projFar = float(getWorld()->getCameraManipulator()->getProjectionFar());

    std::string loadingTextString = osgGaming::PropertiesManager::getInstance()->getValue<std::string>(Param_LocalizationInfoTextLoading);

    _loadingText = new UIText();
    _loadingText->setText(loadingTextString);
    _loadingText->setFontSize(25);
    _loadingText->setVerticalAlignment(UIElement::BOTTOM);
    _loadingText->setMargin(10.0f);

    getHud()->getRootUIElement()->addChild(_loadingText);

    getView(0)->setClampColorEnabled(true);

    getView(0)->addPostProcessingEffect(new FastApproximateAntiAliasingEffect(getView(0)->getResolution()));
    getView(0)->addPostProcessingEffect(new HighDynamicRangeEffect());
    getView(0)->addPostProcessingEffect(new DepthOfFieldEffect(projNear, projFar), false);
  }

  GameState::StateEvent* LoadingGlobeOverviewState::update()
  {
    int dotCount = int(getSimulationTime() * 10.0) % 4;
    std::string loadingTextString = osgGaming::PropertiesManager::getInstance()->getValue<std::string>(Param_LocalizationInfoTextLoading);

    for (int i = 0; i < dotCount; i++)
      loadingTextString += ".";

    _loadingText->setText(loadingTextString);

    return stateEvent_default();
  }

  void LoadingGlobeOverviewState::load(ref_ptr<World> world, osg::ref_ptr<Hud> hud, ref_ptr<GameSettings> settings)
  {
    PropertiesManager::getInstance()->loadPropertiesFromXmlResource("./GameData/data/game_parameters.xml");

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