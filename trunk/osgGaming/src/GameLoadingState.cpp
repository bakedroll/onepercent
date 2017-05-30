#include <osgGaming/GameLoadingState.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/GameState.h>
#include <osgGaming/Hud.h>
#include <osgGaming/View.h>
#include <osgGaming/World.h>

namespace osgGaming
{

  GameLoadingState::GameLoadingState(osg::ref_ptr<GameState> nextState)
    : AbstractGameState()
  {
    _nextStates.push_back(nextState);
  }

  GameLoadingState::GameLoadingState(AbstractGameState::AbstractGameStateList nextStates)
    : AbstractGameState(),
    _nextStates(nextStates)
  {

  }

  void GameLoadingState::loading_thread(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings)
  {
    load(world, hud, settings);
  }

  bool GameLoadingState::isLoadingState()
  {
    return true;
  }

  osg::ref_ptr<World> GameLoadingState::overrideWorld(osg::ref_ptr<View> view)
  {
    return new World();
  }

  osg::ref_ptr<Hud> GameLoadingState::overrideHud(osg::ref_ptr<View> view)
  {
    return new Hud();
  }

  AbstractGameState::AbstractGameStateList GameLoadingState::getNextStates()
  {
    return _nextStates;
  }

}