#pragma once

#include <osg/Referenced>
#include <osg/Projection>
#include <osg/Geode>

namespace osgGaming
{
	class Hud : public osg::Referenced
	{
	public:
		Hud();

		osg::ref_ptr<osg::Projection> getProjection();

		// TODO: remove later
		osg::ref_ptr<osg::Geode> getGeode();

	private:
		osg::ref_ptr<osg::Projection> _projection;
		osg::ref_ptr<osg::Geode> _geode;
	};
}