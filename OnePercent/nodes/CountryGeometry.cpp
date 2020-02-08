#include "CountryGeometry.h"

namespace onep
{
  CountryGeometry::CountryGeometry(
    const osg::ref_ptr<osg::Vec3Array>& vertices,
    const osg::ref_ptr<osg::DrawElementsUInt>& triangles,
    const std::vector<osg::ref_ptr<osg::Array>>& texcoords)
      : osg::Geometry()
  {
    Geometry::setUseVertexBufferObjects(true);
    setVertexArray(vertices);

    auto index=0;
    for (const auto& coords : texcoords)
    {
        setTexCoordArray(index++, coords, osg::Array::BIND_PER_VERTEX);
    }

    addPrimitiveSet(triangles);
  }

  CountryGeometry::~CountryGeometry() = default;
}