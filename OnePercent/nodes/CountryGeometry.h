#pragma once

#include <osg/Geometry>

namespace onep
{
  class CountryGeometry : public osg::Geometry
  {
  public:
    CountryGeometry(
      const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::Vec2Array>& texcoords1,
      const osg::ref_ptr<osg::Vec3Array>& texcoords2,
      const osg::ref_ptr<osg::DrawElementsUInt>& triangles);

    ~CountryGeometry();

  };
}