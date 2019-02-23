#pragma once

#include "scripting/LuaConfig.h"
#include "scripting/LuaVisualOsgNode.h"

#include <osg/Geode>

#include <memory>

namespace onep
{
  class LuaStateManager;

  class CountryNode : public LuaVisualOsgNode<osg::Geode>
  {
  public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

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
      const osg::ref_ptr<LuaConfig>& configManager,
      const osg::ref_ptr<LuaStateManager>& lua,
      const std::string& countryName,
      const osg::Vec2f& centerLatLong,
      const osg::Vec2f& size,
      const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::Vec2Array>& texcoords1,
      const osg::ref_ptr<osg::Vec3Array>& texcoords2,
      const osg::ref_ptr<osg::DrawElementsUInt>& triangles,
      BorderIdMap& neighbourBorders);

    ~CountryNode() override;

    void addNeighbor(const osg::ref_ptr<CountryNode>& mesh);
    List& getNeighborCountryNodes() const;

    const BorderIdMap& getNeighborBorders() const;
    const std::vector<int>& getNeighborBorderIds(int neighborId) const;

    bool getIsOnOcean() const;
    std::string getCountryName() const;

    osg::Vec2f getCenterLatLong() const;
    osg::Vec2f getSize() const;
    osg::Vec2f getSurfaceSize() const;
    float getOptimalCameraDistance(float angle, float ratio) const;

    luabridge::LuaRef luaGetNeighbours() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}