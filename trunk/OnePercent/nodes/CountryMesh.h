#pragma once

#include <osg/Geode>

#include "simulation/CountryData.h"

#include <memory>

namespace onep
{
  class CountryMesh : public osg::Geode
  {
  public:
    typedef std::map<int, std::vector<int>> BorderIdMap;

    typedef osg::ref_ptr<CountryMesh> Ptr;
    typedef std::map<int, Ptr> Map;
    typedef std::vector<Ptr> List;

    typedef enum _colorMode
    {
      MODE_SELECTED = 1,
      MODE_NEIGHBOR = 2,
      MODE_HIGHLIGHT_BANKS = 3,
      MODE_HIGHLIGHT_CONTROL = 4,
      MODE_HIGHLIGHT_MEDIA = 5,
      MODE_HIGHLIGHT_CONCERNS = 6,
      MODE_HIGHLIGHT_POLITICS = 7,
    } ColorMode;

    CountryMesh(
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords,
      osg::ref_ptr<osg::DrawElementsUInt> triangles,
      osg::ref_ptr<osg::Program> program,
      BorderIdMap& neighbourBorders);

    ~CountryMesh();

    void addNeighbor(osg::ref_ptr<CountryMesh> mesh, NeighborCountryInfo::Ptr info);

    CountryData::Ptr getCountryData();
    List& getNeighborCountryMeshs();

    const BorderIdMap& getNeighborBorders() const;
    const std::vector<int>& getNeighborBorderIds(int neighborId);

    bool getIsOnOcean() const;

    void setColorMode(ColorMode mode);
    void setCountryData(CountryData::Ptr country);

    void setDistanceShaderEnabled(bool enabled);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}