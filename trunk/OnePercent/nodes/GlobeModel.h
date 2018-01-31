#pragma once

#include <memory>

#include <osg/Geometry>

#include <osgGaming/Injector.h>
#include <osgGaming/TransformableCameraManipulator.h>

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
    typedef osg::ref_ptr<GlobeModel> Ptr;

		GlobeModel(osgGaming::Injector& injector);
    ~GlobeModel();

    void loadFromDisk(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

		void updateLightDirection(osg::Vec3f direction);
		void updateClouds(float day);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
