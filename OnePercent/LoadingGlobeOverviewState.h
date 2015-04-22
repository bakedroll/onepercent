#pragma once

#include <osgGaming/GameLoadingState.h>
#include <osgGaming/World.h>
#include <osgGaming/GameSettings.h>
#include <osgText/Text>

namespace onep
{
	class LoadingGlobeOverviewState : public osgGaming::GameLoadingState
	{
	public:
		LoadingGlobeOverviewState(osg::ref_ptr<osgGaming::GameState> nextState);

		virtual void initialize() override;
		virtual osgGaming::StateEvent* update() override;

		virtual void load(osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::GameSettings> settings) override;

	private:
		osg::ref_ptr<osgText::Text> _loadingText;

	};
}