#pragma once

#include "scripting/LuaConfig.h"
#include "scripting/LuaVisualOsgNode.h"

#include <osg/Geode>

#include <memory>

#include <luaHelper/ILuaStateManager.h>

namespace onep
{
  class CountryNode : public LuaVisualOsgNode<osg::Geode>
  {
  public:
    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using Ptr  = osg::ref_ptr<CountryNode>;
    using Map  = std::map<int, Ptr>;
    using List = std::vector<Ptr>;

    CountryNode(
      const osg::ref_ptr<luaHelper::ILuaStateManager>& lua,
      const std::string& countryName,
      const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::Vec2Array>& texcoordsPolar,
      const osg::ref_ptr<osg::Vec3Array>& texcoordsCartesian,
      const osg::ref_ptr<osg::Vec2Array>& texcoordsProjected,
      const osg::ref_ptr<osg::DrawElementsUInt>& triangles);

    ~CountryNode() override;

    void addNeighbor(const osg::ref_ptr<CountryNode>& mesh);
    List& getNeighborCountryNodes() const;

    std::string getCountryName() const;

    luabridge::LuaRef luaGetNeighbours() const;

    void clearNeighbors();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
