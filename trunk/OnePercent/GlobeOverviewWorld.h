#pragma once

#include <osgGaming/World.h>

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		GlobeOverviewWorld();

		osg::Vec2f getTimeOfYearAndDay();
		
		void setTimeOfYearAndDay(osg::Vec2f timeOfYearAndDay);

	private:
		osg::Vec2f _timeOfYearAndDay;

	};
}