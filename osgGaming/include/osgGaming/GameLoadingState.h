#pragma once

#include <osgGaming/AbstractGameState.h>
#include <osgGaming/GameState.h>

namespace osgGaming
{
	class GameLoadingState : public AbstractGameState
	{
	public:
		GameLoadingState(osg::ref_ptr<GameState> nextState);
		GameLoadingState(AbstractGameState::AbstractGameStateList nextStates);

		void loading_thread(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings);
		virtual void load(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings) = 0;

		virtual bool isLoadingState() override final;

		AbstractGameState::AbstractGameStateList getNextStates();

	protected:
		virtual osg::ref_ptr<World> newWorld() override;
		virtual osg::ref_ptr<Hud> newHud() override;

	private:
		AbstractGameState::AbstractGameStateList _nextStates;
	};
}