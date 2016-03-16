#pragma once

#include <osg/Geode>
#include <osg/Material>

#include "simulation/CountryData.h"

namespace onep
{
  class CountryMesh : public osg::Geode
  {
  public:
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

    void addNeighbor(osg::ref_ptr<CountryMesh> mesh, NeighborCountryInfo::Ptr info);

    CountryData::Ptr getCountryData();
    List& getNeighborCountryMeshs();

    void setColorMode(ColorMode mode);
    void setCountryData(CountryData::Ptr country);

  private:
    osg::ref_ptr<osg::Material> m_material;
    CountryData::Ptr m_countryData;

    List m_neighbors;
  };
}