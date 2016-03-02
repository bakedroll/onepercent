#include "CountryMesh.h"
#include <osg/Geometry>

namespace onep
{
  CountryMesh::CountryMesh(
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::DrawElementsUInt> triangles)
    : Geode()
  {
    osg::ref_ptr<osg::Geometry> geo = new osg::Geometry();
    geo->setVertexArray(vertices);
    geo->addPrimitiveSet(triangles);

    addDrawable(geo);

    m_material = new osg::Material();
    m_material->setColorMode(osg::Material::DIFFUSE);
    getOrCreateStateSet()->setAttribute(m_material, osg::StateAttribute::ON);
  }

  void CountryMesh::addNeighborCountry(osg::ref_ptr<CountryMesh> mesh, osg::ref_ptr<NeighborCountryInfo> info)
  {
    Neighbor nc;
    nc.mesh = mesh;
    nc.info = info;

    m_neighbours.push_back(nc);
  }

  Country::Ptr CountryMesh::getCountryData()
  {
    return m_countryData;
  }

  CountryMesh::_neighbor::List& CountryMesh::getNeighborCountryMeshs()
  {
    return m_neighbours;
  }

  void CountryMesh::setColorMode(ColorMode mode)
  {
    switch (mode)
    {
    case MODE_SELECTED:
      m_material->setDiffuse(osg::Material::FRONT, osg::Vec4f(1.0f, 0.0f, 0.0f, 0.5f));
      break;
    case MODE_NEIGHBOR:
      m_material->setDiffuse(osg::Material::FRONT, osg::Vec4f(1.0f, 1.0f, 0.0f, 0.3f));
      break;
    default:
      break;
    }
  }

  void CountryMesh::setCountryData(Country::Ptr country)
  {
    m_countryData = country;
  }
}