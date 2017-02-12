#pragma once

#include <osgGaming/GameLoadingState.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/GameState.h>
#include <osgGaming/UIText.h>
#include <osgGaming/World.h>

namespace onep
{
	class LoadingGlobeOverviewState : public osgGaming::GameLoadingState
	{
	public:
		LoadingGlobeOverviewState(osg::ref_ptr<osgGaming::GameState> nextState);
		LoadingGlobeOverviewState(osgGaming::AbstractGameState::AbstractGameStateList nextStates);

		virtual void initialize() override;
		virtual osgGaming::GameState::StateEvent* update() override;

		virtual void load(osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::Hud> hud, osg::ref_ptr<osgGaming::GameSettings> settings) override;

	private:
		osg::ref_ptr<osgGaming::UIText> _loadingText;

	};
}