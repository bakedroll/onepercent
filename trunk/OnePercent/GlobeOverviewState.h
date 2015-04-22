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

		virtual void onKeyPressedEvent(int key) override;
		virtual void onMousePressedEvent(int button, float x, float y) override;

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position) override;

	private:
		double _globeAngle;

		osg::ref_ptr<GlobeOverviewWorld> _globeWorld;
	};
}