#pragma once

#include "GlobeModel.h"
#include "BackgroundModel.h"
#include "CountryNameOverlay.h"

#include "simulation/Simulation.h"

#include <osgGaming/World.h>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		GlobeOverviewWorld();
    ~GlobeOverviewWorld();

		osg::ref_ptr<GlobeModel> getGlobeModel();
		osg::ref_ptr<CountryNameOverlay> getCountryOverlay();
		osg::ref_ptr<BackgroundModel> getBackgroundModel();
		osg::ref_ptr<Simulation> getSimulation();
		
		void setGlobeModel(osg::ref_ptr<GlobeModel> globeModel);
		void setCountryOverlay(osg::ref_ptr<CountryNameOverlay> countryOverlay);
		void setBackgroundModel(osg::ref_ptr<BackgroundModel> backgroundModel);

		void setDay(float day);

		osg::Vec2f getCameraLatLong();
		osg::Vec2f getCameraViewAngle();
		float getCameraDistance();

		void updateCameraPosition(osg::Vec2f latLong, osg::Vec2f viewAngle, float distance);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}