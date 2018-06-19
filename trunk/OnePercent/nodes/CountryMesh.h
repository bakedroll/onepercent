#pragma once

#include "scripting/ConfigManager.h"

#include <osg/Geode>

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
      MODE_HOVER = 8
    } ColorMode;

    CountryMesh(
      osg::ref_ptr<ConfigManager> configManager,
      osg::Vec2f centerLatLong,
      osg::Vec2f size,
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords,
      osg::ref_ptr<osg::DrawElementsUInt> triangles,
      BorderIdMap& neighbourBorders);

    ~CountryMesh();

    void addNeighbor(osg::ref_ptr<CountryMesh> mesh);
    List& getNeighborCountryMeshs();

    const BorderIdMap& getNeighborBorders() const;
    const std::vector<int>& getNeighborBorderIds(int neighborId);

    bool getIsOnOcean() const;

    void setColorMode(ColorMode mode);
    void setHoverMode(bool bHoverEnabled);

    osg::Vec2f getCenterLatLong();
    osg::Vec2f getSize();
    osg::Vec2f getSurfaceSize();
    float getOptimalCameraDistance(float angle, float ratio);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}