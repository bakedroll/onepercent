#pragma once

#include "GlobeModel.h"

#include <osgGaming/World.h>

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		GlobeOverviewWorld();

		osg::ref_ptr<GlobeModel> getGlobeModel();
		osg::Vec2f getTimeOfYearAndDay();
		
		void setGlobeModel(osg::ref_ptr<GlobeModel> globeModel);
		void setTimeOfYearAndDay(osg::Vec2f timeOfYearAndDay);

	private:
		osg::Vec2f _timeOfYearAndDay;

		osg::ref_ptr<GlobeModel> _globeModel;
	};
}