#pragma once

#include "GlobeModel.h"
#include "BackgroundModel.h"

#include <osgGaming/World.h>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		GlobeOverviewWorld();

		osg::ref_ptr<GlobeModel> getGlobeModel();
		osg::ref_ptr<BackgroundModel> getBackgroundModel();
		osg::Vec2f getTimeOfYearAndDay();
		
		void setGlobeModel(osg::ref_ptr<GlobeModel> globeModel);
		void setBackgroundModel(osg::ref_ptr<BackgroundModel> backgroundModel);
		osg::Vec3f setTimeOfYearAndDay(osg::Vec2f timeOfYearAndDay);

		void updateSun(osg::Vec3f sunDirection);

	private:
		osg::Vec2f _timeOfYearAndDay;

		osg::ref_ptr<GlobeModel> _globeModel;
		osg::ref_ptr<BackgroundModel> _backgroundModel;
	};
}