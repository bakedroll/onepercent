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
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    typedef std::map<int, std::vector<int>> BorderIdMap;

    using Ptr  = osg::ref_ptr<CountryNode>;
    using Map  = std::map<int, Ptr>;
    using List = std::vector<Ptr>;

    CountryNode(
      const osg::ref_ptr<LuaStateManager>& lua,
      const std::string& countryName,
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

    luabridge::LuaRef luaGetNeighbours() const;

    void clearNeighbors();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}