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

		osg::ref_ptr<osg::Projection> getProjection() const;
		osg::ref_ptr<osg::MatrixTransform> getModelViewTransform() const;

		void updateResolution(const osg::Vec2f& resolution);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
