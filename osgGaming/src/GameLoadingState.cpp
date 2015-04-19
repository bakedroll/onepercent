#include <osgGaming/GameLoadingState.h>

using namespace osg;
using namespace osgGaming;

void GameLoadingState::load(osg::ref_ptr<World>, ref_ptr<GameSettings> settings)
{

}

bool GameLoadingState::isLoadingState()
{
	return true;
}