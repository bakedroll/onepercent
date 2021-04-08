#pragma once

#include "GlobeModel.h"

#include <osgHelper/LightingNode.h>
#include <osgHelper/Camera.h>

namespace onep
{
	class GlobeOverviewWorld : public osgHelper::LightingNode
	{
	public:
		GlobeOverviewWorld(osgHelper::ioc::Injector& injector);
    ~GlobeOverviewWorld();

    void initialize();
		
		void setDay(const osgHelper::Camera::Ptr& camera, float day);

		osg::Vec2f getCameraLatLong();
		osg::Vec2f getCameraViewAngle();
		float getCameraDistance();

    void updateCameraPosition(const osgHelper::Camera::Ptr& camera, osg::Vec2f latLong, osg::Vec2f viewAngle,
                              float distance);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}