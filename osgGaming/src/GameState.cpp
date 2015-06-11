#include <osgGaming/GameState.h>

using namespace osgGaming;

GameState::GameState()
	: AbstractGameState()
{

}

bool GameState::isLoadingState()
{
	return false;
}