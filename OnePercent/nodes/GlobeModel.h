#pragma once

#include "core/Globals.h"

#include <osg/Geometry>

#include <osgGaming/TransformableCameraManipulator.h>

#include "nodes/BoundariesMesh.h"
#include "nodes/CountryOverlay.h"

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
    typedef osg::ref_ptr<GlobeModel> Ptr;

		GlobeModel(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);
    ~GlobeModel();

		void updateLightDirection(osg::Vec3f direction);
		void updateClouds(float day);

    BoundariesMesh::Ptr getBoundariesMesh();
    CountryOverlay::Ptr getCountryOverlay();

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
