#pragma once

#include "scripting/LuaConfig.h"

#include <osg/Geode>

#include <memory>

namespace onep
{
  class CountryNode : public osg::Geode
  {
  public:
    typedef std::map<int, std::vector<int>> BorderIdMap;

    typedef osg::ref_ptr<CountryNode> Ptr;
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
      MODE_HIGHLIGHT_POLITICS = 7
    } ColorMode;

    CountryNode(
      osg::ref_ptr<LuaConfig> configManager,
      osg::Vec2f centerLatLong,
      osg::Vec2f size,
      osg::ref_ptr<osg::Vec3Array> vertices,
      osg::ref_ptr<osg::Vec2Array> texcoords1,
      osg::ref_ptr<osg::Vec3Array> texcoords2,
      osg::ref_ptr<osg::DrawElementsUInt> triangles,
      BorderIdMap& neighbourBorders);

    ~CountryNode();

    void addNeighbor(osg::ref_ptr<CountryNode> mesh);
    List& getNeighborCountryNodes() const;

    const BorderIdMap& getNeighborBorders() const;
    const std::vector<int>& getNeighborBorderIds(int neighborId) const;

    bool getIsOnOcean() const;

    void setColorMode(ColorMode mode);

    osg::Vec2f getCenterLatLong() const;
    osg::Vec2f getSize() const;
    osg::Vec2f getSurfaceSize() const;
    float getOptimalCameraDistance(float angle, float ratio) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}