#pragma once

#include <osgGaming/GameState.h>

namespace osgGaming
{
	class GameLoadingState : public GameState
	{
	public:
		GameLoadingState(osg::ref_ptr<GameState> nextState);

		virtual void load(osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings);

		virtual bool isLoadingState() override;

		osg::ref_ptr<GameState> getNextState();

	private:
		osg::ref_ptr<GameState> _nextState;
	};
}