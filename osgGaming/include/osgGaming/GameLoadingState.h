#pragma once

#include <osgGaming/GameState.h>

namespace osgGaming
{
	class GameLoadingState : public GameState
	{
	public:
		GameLoadingState(osg::ref_ptr<GameState> nextState);

		void loading_thread(osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings);
		virtual void load(osg::ref_ptr<World> world, osg::ref_ptr<GameSettings> settings) = 0;

		virtual bool isLoadingState() override final;

		osg::ref_ptr<GameState> getNextState();

	private:
		osg::ref_ptr<GameState> _nextState;
	};
}