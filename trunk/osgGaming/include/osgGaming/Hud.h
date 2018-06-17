#pragma once

#include <osg/Referenced>
#include <osg/Projection>
#include <osg/MatrixTransform>

#include <memory>

namespace osgGaming
{
  class Injector;

	class Hud : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<Hud> Ptr;

		Hud(Injector& injector);
		~Hud();

		osg::ref_ptr<osg::Projection> getProjection();
		osg::ref_ptr<osg::MatrixTransform> getModelViewTransform();

		void updateResolution(osg::Vec2f resolution);
		void setFpsEnabled(bool enabled);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
