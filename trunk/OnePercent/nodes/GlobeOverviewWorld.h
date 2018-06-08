#pragma once

#include "GlobeModel.h"

#include <osgGaming/World.h>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		GlobeOverviewWorld(osgGaming::Injector& injector);
    ~GlobeOverviewWorld();

    void initialize();
		
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