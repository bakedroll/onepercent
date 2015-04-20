#pragma once

#include "GlobeOverviewWorld.h"

#include <osgGaming/GameState.h>

namespace onep
{
	class GlobeOverviewState : public osgGaming::GameState
	{
	public:
		GlobeOverviewState();

		virtual void initialize(osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::GameSettings> settings) override;
		virtual osgGaming::StateEvent* update(double frameTime, osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::GameSettings> settings) override;

		virtual void onKeyHitEvent(int key) override;
		virtual void onMouseHitEvent(int button, float x, float y) override;

	private:
		double _globeAngle;

		osg::ref_ptr<GlobeOverviewWorld> _globeWorld;

		bool _exitGame;
	};
}