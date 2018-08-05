#include "CountryGeometry.h"

namespace onep
{
  CountryGeometry::CountryGeometry(
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::Vec2Array> texcoords1,
    osg::ref_ptr<osg::Vec3Array> texcoords2,
    osg::ref_ptr<osg::DrawElementsUInt> triangles)
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