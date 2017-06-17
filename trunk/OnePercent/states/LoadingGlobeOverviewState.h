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
		LoadingGlobeOverviewState(osg::ref_ptr<osgGaming::GameState> nextState);
		LoadingGlobeOverviewState(osgGaming::AbstractGameState::AbstractGameStateList nextStates);
		~LoadingGlobeOverviewState();

		virtual void initialize() override;
		virtual VirtualOverlay* createVirtualOverlay() override;
		virtual osgGaming::GameState::StateEvent* update() override;

		virtual void load(osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::Hud> hud, osg::ref_ptr<osgGaming::GameSettings> settings) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
