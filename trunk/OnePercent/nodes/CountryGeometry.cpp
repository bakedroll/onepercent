#include "CountryGeometry.h"

namespace onep
{
  CountryGeometry::CountryGeometry(
    const osg::ref_ptr<osg::Vec3Array>& vertices,
    const osg::ref_ptr<osg::Vec2Array>& texcoords1,
    const osg::ref_ptr<osg::Vec3Array>& texcoords2,
    const osg::ref_ptr<osg::DrawElementsUInt>& triangles)
  {
    setUseVertexBufferObjects(true);
    setVertexArray(vertices);

    if (texcoords1.valid())
      setTexCoordArray(0, texcoords1, osg::Array::BIND_PER_VERTEX);

    if (texcoords2.valid())
      setTexCoordArray(1, texcoords2, osg::Array::BIND_PER_VERTEX);

    addPrimitiveSet(triangles);
  }

  CountryGeometry::~CountryGeometry()
  {
  }
}