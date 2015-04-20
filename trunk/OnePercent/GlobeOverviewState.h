#pragma once

#include <osgGaming/GameState.h>

namespace onep
{
	class GlobeOverviewState : public osgGaming::GameState
	{
	public:
		GlobeOverviewState();

		virtual osgGaming::StateEvent* update(double frameTime, osg::ref_ptr<osgGaming::World> world, osg::ref_ptr<osgGaming::GameSettings> settings);

		virtual void onKeyDownEvent(int key);

	private:
		double _globeAngle;
	};
}