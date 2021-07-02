#include "CountryNode.h"

#include "nodes/CountryGeometry.h"

#include <osg/Geometry>
#include <osgHelper/Helper.h>

#include <cassert>

namespace onep
{
  void CountryNode::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .deriveClass<CountryNode, LuaVisualOsgNode<osg::Geode>>("CountryNode")
      .addFunction("get_country_name", &CountryNode::getCountryName)
      .addFunction("get_neighbours", &CountryNode::luaGetNeighbours)
      .endClass();
  }

  struct CountryNode::Impl
  {
    Impl() = default;

    std::string name;
    List        neighbors;

    luaHelper::LuaRefPtr refNeighbours;
  };

  CountryNode::CountryNode(
    const luaHelper::LuaStateManager::Ptr& lua,
    const std::string& countryName,
    const osg::ref_ptr<osg::Vec3Array>& vertices,
    const osg::ref_ptr<osg::Vec2Array>& texcoordsPolar,
    const osg::ref_ptr<osg::Vec3Array>& texcoordsCartesian,
    const osg::ref_ptr<osg::Vec2Array>& texcoordsProjected,
    const osg::ref_ptr<osg::DrawElementsUInt>& triangles)
    : LuaVisualOsgNode<osg::Geode>()
    , m(new Impl())
  {
    m->name = countryName;

    const auto geo = new CountryGeometry(vertices, triangles, { texcoordsPolar, texcoordsCartesian, texcoordsProjected });
    Geode::addDrawable(geo);

    addStateSetUniform(new osg::Uniform("overlayColor", osg::Vec4f(0.3f, 0.3f, 0.3f, 0.0f)));
    addStateSetUniform(new osg::Uniform("takeover", 0.0f));

    m->refNeighbours = MAKE_LUAREF_PTR(lua->newTable());
  }

  CountryNode::~CountryNode() = default;

  void CountryNode::addNeighbor(const osg::ref_ptr<CountryNode>& mesh)
  {
    m->refNeighbours->append(mesh.get());
    m->neighbors.push_back(mesh);
  }

  CountryNode::List& CountryNode::getNeighborCountryNodes() const
  {
    return m->neighbors;
  }

  std::string CountryNode::getCountryName() const
  {
    return m->name;
  }

  luabridge::LuaRef CountryNode::luaGetNeighbours() const
  {
    return *m->refNeighbours.get();
  }

  void CountryNode::clearNeighbors()
  {
    m->neighbors.clear();
  }
}
