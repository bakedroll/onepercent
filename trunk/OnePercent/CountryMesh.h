#pragma once

#include <osg/Geode>
#include <osg/Material>
#include "Country.h"

namespace onep
{
  class CountryMesh : public osg::Geode
  {
  public:
    typedef struct _neighborCountryMesh
    {
      typedef std::vector<_neighborCountryMesh> List;

      osg::ref_ptr<CountryMesh> mesh;
      osg::ref_ptr<NeighborCountryInfo> info;
    } NeighborCountryMesh;

    typedef osg::ref_ptr<CountryMesh> Ptr;
    typedef std::map<int, Ptr> Map;
    typedef std::vector<Ptr> List;

    typedef enum _colorMode
    {
      MODE_SELECTED,
      MODE_NEIGHBOR
    } ColorMode;

    CountryMesh(
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::DrawElementsUInt> triangles);

    void addNeighborCountry(osg::ref_ptr<CountryMesh> mesh, osg::ref_ptr<NeighborCountryInfo> info);

    Country::Ptr getCountryData();
    NeighborCountryMesh::List& getNeighborCountryMeshs();

    void setColorMode(ColorMode mode);
    void setCountryData(Country::Ptr country);

  private:
    osg::ref_ptr<osg::Material> m_material;
    Country::Ptr m_countryData;

    NeighborCountryMesh::List m_neighbours;
  };
}