#pragma once

#include <osgGaming/AbstractGameState.h>

namespace osgGaming
{
  class Injector;
  class GameState;

	class GameLoadingState : public AbstractGameState
	{
	public:
		GameLoadingState(Injector& container);

		void loading_thread(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings);
		virtual void load(osg::ref_ptr<World> world, osg::ref_ptr<Hud> hud, osg::ref_ptr<GameSettings> settings) = 0;

		virtual bool isLoadingState() override final;

    void getNextStates(Injector& injector, AbstractGameStateList& states);

	protected:
		virtual osg::ref_ptr<World> overrideWorld(osg::ref_ptr<View> view) override;
    virtual osg::ref_ptr<Hud> overrideHud(osg::ref_ptr<View> view) override;

    virtual void injectNextStates(Injector& injector, AbstractGameStateList& states) = 0;

  private:
    bool m_bStatesInjected;
    AbstractGameStateList m_nextStates;

	};
}