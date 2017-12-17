#include "CountryMesh.h"

#include <osg/Geometry>
#include <osg/Material>

namespace onep
{
  struct CountryMesh::Impl
  {
    Impl() {}

    osg::ref_ptr<osg::Material> material;
    CountryData::Ptr countryData;

    List neighbors;
    BorderIdMap neighbourBorders;
  };

  CountryMesh::CountryMesh(
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::DrawElementsUInt> triangles,
    BorderIdMap& neighbourBorders)
    : osg::Geode()
    , m(new Impl())
  {
    m->neighbourBorders = neighbourBorders;

    osg::ref_ptr<osg::Geometry> geo = new osg::Geometry();
    geo->setVertexArray(vertices);
    geo->addPrimitiveSet(triangles);

    addDrawable(geo);

    m->material = new osg::Material();
    m->material->setColorMode(osg::Material::DIFFUSE);
    getOrCreateStateSet()->setAttribute(m->material, osg::StateAttribute::ON);
  }

  CountryMesh::~CountryMesh()
  {
  }

  void CountryMesh::addNeighbor(osg::ref_ptr<CountryMesh> mesh, NeighborCountryInfo::Ptr info)
  {
    CountryData::Neighbor nb;
    nb.country = mesh->getCountryData();
    nb.info = info;

    m->neighbors.push_back(mesh);
    m->countryData->addNeighbor(nb);
  }

  CountryData::Ptr CountryMesh::getCountryData()
  {
    return m->countryData;
  }

  CountryMesh::List& CountryMesh::getNeighborCountryMeshs()
  {
    return m->neighbors;
  }

  const CountryMesh::BorderIdMap& CountryMesh::getNeighborBorders() const
  {
    return m->neighbourBorders;
  }

  const std::vector<int>& CountryMesh::getNeighborBorderIds(int neighborId)
  {
    BorderIdMap::iterator it = m->neighbourBorders.find(neighborId);
    if (it == m->neighbourBorders.end())
    {
      assert(false);
      return m->neighbourBorders[neighborId];
    }

    return it->second;
  }

  bool CountryMesh::getIsOnOcean() const
  {
    return m->neighbourBorders.find(-1) != m->neighbourBorders.end();
  }

  void CountryMesh::setColorMode(ColorMode mode)
  {
    switch (mode)
    {
    case MODE_SELECTED:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(1.0f, 0.0f, 0.0f, 0.5f));
      break;
    case MODE_NEIGHBOR:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(1.0f, 1.0f, 0.0f, 0.3f));
      break;
    case MODE_HIGHLIGHT_BANKS:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(0.0f, 0.0f, 0.8f, 0.3f));
      break;
    case MODE_HIGHLIGHT_CONTROL:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(0.635f, 0.439f, 0.031f, 0.3f));
      break;
    case MODE_HIGHLIGHT_CONCERNS:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(0.118f, 0.753f, 0.208f, 0.3f));
      break;
    case MODE_HIGHLIGHT_MEDIA:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(0.902f, 1.0f, 0.357f, 0.3f));
      break;
    case MODE_HIGHLIGHT_POLITICS:
      m->material->setDiffuse(osg::Material::FRONT, osg::Vec4f(0.69f, 0.247f, 0.624f, 0.3f));
      break;
    default:
      break;
    }
  }

  void CountryMesh::setCountryData(CountryData::Ptr country)
  {
    m->countryData = country;
  }
}