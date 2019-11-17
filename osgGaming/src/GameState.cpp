#include <osgGaming/GameState.h>

namespace osgGaming
{

  GameState::GameState()
	  : AbstractGameState()
  {

  }

  bool GameState::isLoadingState() const
  {
	  return false;
}

}