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

  void CountryMesh::addNeighbor(osg::ref_ptr<CountryMesh> mesh, NeighborCountryInfo::Ptr info)
  {
    CountryData::Neighbor nb;
    nb.country = mesh->getCountryData();
    nb.info = info;

    m_neighbors.push_back(mesh);
    m_countryData->addNeighbor(nb);
  }

  CountryData::Ptr CountryMesh::getCountryData()
  {
    return m_countryData;
  }

  CountryMesh::List& CountryMesh::getNeighborCountryMeshs()
  {
    return m_neighbors;
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
    case MODE_HIGHLIGHT_BANKS:
      m_material->setDiffuse(osg::Material::FRONT, osg::Vec4f(0.0f, 0.0f, 0.8f, 0.3f));
      break;
    default:
      break;
    }
  }

  void CountryMesh::setCountryData(CountryData::Ptr country)
  {
    m_countryData = country;
  }
}