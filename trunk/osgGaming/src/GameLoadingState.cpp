#include <osgGaming/GameLoadingState.h>

using namespace osg;
using namespace osgGaming;

GameLoadingState::GameLoadingState(ref_ptr<GameState> nextState)
	: AbstractGameState(),
	  _nextState(nextState)
{

}

void GameLoadingState::loading_thread(ref_ptr<World> world, osg::ref_ptr<Hud> hud, ref_ptr<GameSettings> settings)
{
	load(world, hud, settings);
}

bool GameLoadingState::isLoadingState()
{
	return true;
}

ref_ptr<World> GameLoadingState::newWorld()
{
	return new World();
}

ref_ptr<Hud> GameLoadingState::newHud()
{
	return new Hud();
}

osg::ref_ptr<GameState> GameLoadingState::getNextState()
{
	return _nextState;
}