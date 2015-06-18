#pragma once

#include "GlobeModel.h"
#include "BackgroundModel.h"
#include "Simulation.h"

#include <osgGaming/World.h>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		GlobeOverviewWorld();

		osg::ref_ptr<GlobeModel> getGlobeModel();
		osg::ref_ptr<BackgroundModel> getBackgroundModel();
		osg::ref_ptr<Simulation> getSimulation();
		
		void setGlobeModel(osg::ref_ptr<GlobeModel> globeModel);
		void setBackgroundModel(osg::ref_ptr<BackgroundModel> backgroundModel);

		void setDay(float day);

	private:
		static const float _DAYS_IN_YEAR;

		void updateSun(osg::Vec3f sunDirection);

		osg::ref_ptr<GlobeModel> _globeModel;
		osg::ref_ptr<BackgroundModel> _backgroundModel;

		osg::ref_ptr<Simulation> _simulation;
	};
}