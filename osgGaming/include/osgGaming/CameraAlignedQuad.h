#pragma once

#include <osg/Geode>
#include <osg/Geometry>

namespace osgGaming
{
	class CameraAlignedQuad : public osg::Geode
	{
	public:
		CameraAlignedQuad(int renderBin = 10);

		osg::ref_ptr<osg::Geometry> getGeometry();
		osg::ref_ptr<osg::Vec3Array> getVertexArray();
		osg::ref_ptr<osg::Vec3Array> getNormalArray();

	private:
		void makeQuad(int renderBin);

		osg::ref_ptr<osg::Geometry> _geometry;
		osg::ref_ptr<osg::Vec3Array> _vertexArray;
		osg::ref_ptr<osg::Vec3Array> _normalArray;
	};
}