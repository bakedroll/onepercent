#pragma once

#include <osgGaming/AbstractGameState.h>

namespace osgGaming
{
  class GameState;

	class GameLoadingState : public AbstractGameState
	{
	public:
		GameLoadingState(osg::ref_ptr<GameState> nextState);
		GameLoadingState(AbstractGameStateList nextStates);

		void loading_thread(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings);
		virtual void load(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings) = 0;

		virtual bool isLoadingState() override final;

		AbstractGameStateList getNextStates();

	protected:
		virtual osg::ref_ptr<World> overrideWorld() override;
		virtual osg::ref_ptr<Hud> overrideHud() override;

	private:
		AbstractGameStateList _nextStates;
	};
}