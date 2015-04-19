#pragma once

#include <osgGaming/World.h>

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		osg::ref_ptr<osg::PositionAttitudeTransform> getGlobeTransform();
		
		void setGlobeTransform(osg::ref_ptr<osg::PositionAttitudeTransform> transform);

	private:
		osg::ref_ptr<osg::PositionAttitudeTransform> _globeTransform;

	};
}