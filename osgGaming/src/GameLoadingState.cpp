#include <osgGaming/GameLoadingState.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/Hud.h>
#include <osgGaming/View.h>
#include <osgGaming/World.h>

namespace osgGaming
{
  GameLoadingState::GameLoadingState(Injector& container)
    : AbstractGameState()
    , m_bStatesInjected(false)
  {
  }

  void GameLoadingState::loading_thread(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud,
                                        osg::ref_ptr<GameSettings> settings)
  {
    load(world, hud, settings);
  }

  bool GameLoadingState::isLoadingState() const
  {
    return true;
  }

  osg::ref_ptr<World> GameLoadingState::injectWorld(osgGaming::Injector& injector, const osg::ref_ptr<View>& view)
  {
    return injector.inject<World>();
  }

  osg::ref_ptr<Hud> GameLoadingState::injectHud(osgGaming::Injector& injector, const osg::ref_ptr<View>& view)
  {
    return injector.inject<Hud>();
  }

  void GameLoadingState::getNextStates(Injector& injector, AbstractGameStateList& states)
  {
    if (m_bStatesInjected)
    {
      states = m_nextStates;
      return;
    }

    injectNextStates(injector, states);
    m_nextStates = states;
    m_bStatesInjected = true;
  }

}