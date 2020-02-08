#pragma once

#include <osg/Geometry>

namespace onep
{
  class CountryGeometry : public osg::Geometry
  {
  public:
    CountryGeometry(
      const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::DrawElementsUInt>& triangles,
      const std::vector<osg::ref_ptr<osg::Array>>& texcoords = {});

    ~CountryGeometry();

  };
}