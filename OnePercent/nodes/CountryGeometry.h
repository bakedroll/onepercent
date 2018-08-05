#pragma once

#include <osg/Geometry>

namespace onep
{
  class CountryGeometry : public osg::Geometry
  {
  public:
    CountryGeometry(
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords1,
      osg::ref_ptr<osg::Vec3Array> texcoords2,
      osg::ref_ptr<osg::DrawElementsUInt> triangles);

    ~CountryGeometry();

  };
}