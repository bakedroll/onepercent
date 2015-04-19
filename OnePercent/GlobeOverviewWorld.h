#pragma once

#include <osgGaming/World.h>

#include <osg/PositionAttitudeTransform>

namespace onep
{
	class GlobeOverviewWorld : public osgGaming::World
	{
	public:
		void initialize();

		osg::ref_ptr<osg::PositionAttitudeTransform> getGlobeTransform();

	private:
		osg::ref_ptr<osg::PositionAttitudeTransform> _globeTransform;

	};
}