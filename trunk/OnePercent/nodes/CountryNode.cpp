#include "CountryNode.h"

#include "nodes/CountryGeometry.h"
#include "scripting/LuaStateManager.h"

#include <osg/Geometry>
#include <osgGaming/Helper.h>

#include <algorithm>
#include <cassert>

namespace onep
{
  void CountryNode::Definition::registerClass(lua_State* state)
  {
    getGlobalNamespace(state)
      .deriveClass<CountryNode, LuaVisualOsgNode<osg::Geode>>("CountryNode")
      .addFunction("get_country_name", &CountryNode::getCountryName)
      .addFunction("get_neighbours", &CountryNode::luaGetNeighbours)
      .endClass();
  }

  struct CountryNode::Impl
  {
    Impl()
      : earthRadius(0.0f)
      , cameraZoom(0.0f)
    {
    }

    std::string name;

    osg::Vec2f centerLatLong;
    osg::Vec2f size;

    float earthRadius;
    float cameraZoom;

    List        neighbors;
    BorderIdMap neighbourBorders;
    LuaRefPtr   refNeighbours;
  };

  CountryNode::CountryNode(
    const LuaConfig::Ptr& configManager,
    const LuaStateManager::Ptr& lua,
    const std::string& countryName,
    const osg::Vec2f& centerLatLong,
    const osg::Vec2f& size,
    const osg::ref_ptr<osg::Vec3Array>& vertices,
    const osg::ref_ptr<osg::Vec2Array>& texcoords1,
    const osg::ref_ptr<osg::Vec3Array>& texcoords2,
    const osg::ref_ptr<osg::DrawElementsUInt>& triangles,
    BorderIdMap& neighbourBorders)
    : LuaVisualOsgNode<osg::Geode>()
    , m(new Impl())
  {
    m->name = countryName;

    m->earthRadius = configManager->getNumber<float>("earth.radius");
    m->cameraZoom  = configManager->getNumber<float>("camera.country_zoom");

    m->centerLatLong = centerLatLong;
    m->size          = size;

    m->neighbourBorders = neighbourBorders;

    osg::ref_ptr<CountryGeometry> geo = new CountryGeometry(vertices, texcoords1, texcoords2, triangles);
    addDrawable(geo);

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

  const CountryNode::BorderIdMap& CountryNode::getNeighborBorders() const
  {
    return m->neighbourBorders;
  }

  const std::vector<int>& CountryNode::getNeighborBorderIds(int neighborId) const
  {
    auto it = m->neighbourBorders.find(neighborId);
    if (it == m->neighbourBorders.end())
    {
      assert(false);
      return m->neighbourBorders[neighborId];
    }

    return it->second;
  }

  bool CountryNode::getIsOnOcean() const
  {
    return m->neighbourBorders.find(-1) != m->neighbourBorders.end();
  }

  std::string CountryNode::getCountryName() const
  {
    return m->name;
  }

  osg::Vec2f CountryNode::getCenterLatLong() const
  {
    return m->centerLatLong;
  }

  osg::Vec2f CountryNode::getSize() const
  {
    return m->size;
  }

  osg::Vec2f CountryNode::getSurfaceSize() const
  {
    return osg::Vec2f(
      2.0f * C_PI * sin(C_PI / 2.0f - abs(m->centerLatLong.x())) * m->earthRadius * m->size.x(),
      C_PI * m->earthRadius * m->size.y());
  }

  float CountryNode::getOptimalCameraDistance(float angle, float ratio) const
  {
    osg::Vec2f surfaceSize = getSurfaceSize();

    float hdistance = surfaceSize.x() * m->cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f));
    float vdistance = surfaceSize.y() * m->cameraZoom / (2.0f * tan(angle * C_PI / 360.0f));

    return std::max(hdistance, vdistance);
  }

  luabridge::LuaRef CountryNode::luaGetNeighbours() const
  {
    return *m->refNeighbours.get();
  }
}
