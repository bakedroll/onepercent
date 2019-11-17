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

		bool isLoadingState() const override final;

    void getNextStates(Injector& injector, AbstractGameStateList& states);

	protected:
		osg::ref_ptr<World> injectWorld(osgGaming::Injector& injector, const osg::ref_ptr<View>& view) override;
    osg::ref_ptr<Hud> injectHud(osgGaming::Injector& injector, const osg::ref_ptr<View>& view) override;

    virtual void injectNextStates(Injector& injector, AbstractGameStateList& states) = 0;

  private:
    bool                  m_bStatesInjected;
    AbstractGameStateList m_nextStates;
  };
}