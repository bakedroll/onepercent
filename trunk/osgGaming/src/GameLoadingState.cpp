#include <osgGaming/GameLoadingState.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/GameState.h>
#include <osgGaming/Hud.h>
#include <osgGaming/World.h>

using namespace osg;
using namespace osgGaming;

GameLoadingState::GameLoadingState(ref_ptr<GameState> nextState)
	: AbstractGameState()
{
	_nextStates.push_back(nextState);
}

GameLoadingState::GameLoadingState(AbstractGameState::AbstractGameStateList nextStates)
	: AbstractGameState(),
	  _nextStates(nextStates)
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

ref_ptr<World> GameLoadingState::overrideWorld()
{
	return new World();
}

ref_ptr<Hud> GameLoadingState::overrideHud()
{
	return new Hud();
}

AbstractGameState::AbstractGameStateList GameLoadingState::getNextStates()
{
	return _nextStates;
}