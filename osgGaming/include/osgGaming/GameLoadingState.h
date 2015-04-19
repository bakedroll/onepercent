#pragma once

#include <osgGaming/GameState.h>

namespace osgGaming
{
	class GameLoadingState : public GameState
	{
	public:
		virtual void load(osg::ref_ptr<World>, osg::ref_ptr<GameSettings> settings);

		virtual bool isLoadingState();
	};
}