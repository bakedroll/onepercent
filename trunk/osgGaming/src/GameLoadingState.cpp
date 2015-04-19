#include <osgGaming/GameLoadingState.h>

using namespace osg;
using namespace osgGaming;

GameLoadingState::GameLoadingState(ref_ptr<GameState> nextState)
	: GameState(),
	  _nextState(nextState)
{

}

void GameLoadingState::load(osg::ref_ptr<World> world, ref_ptr<GameSettings> settings)
{

}

bool GameLoadingState::isLoadingState()
{
	return true;
}

osg::ref_ptr<GameState> GameLoadingState::getNextState()
{
	return _nextState;
}