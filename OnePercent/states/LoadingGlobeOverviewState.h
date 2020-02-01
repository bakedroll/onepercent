#pragma once

#include "states/QtGameLoadingState.h"

#include <osgGaming/GameSettings.h>
#include <osgGaming/GameState.h>
#include <osgGaming/World.h>

#include <memory>

namespace onep
{
	class LoadingGlobeOverviewState : public QtGameLoadingState
	{
	public:
    LoadingGlobeOverviewState(osgGaming::Injector& injector);
		~LoadingGlobeOverviewState();

		void initialize() override;
		osgGaming::GameState::StateEvent* update() override;

		void load(osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::Hud> hud, osg::ref_ptr<osgGaming::GameSettings> settings) override;

    void onResizeEvent(float width, float height) override;

  protected:
    void injectNextStates(osgGaming::Injector& injector, AbstractGameStateList& states) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
