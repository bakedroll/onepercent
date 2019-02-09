#include "CountryNode.h"

#include "nodes/CountryGeometry.h"
#include "scripting/LuaStateManager.h"

#include <osg/Geometry>
#include <osgGaming/Helper.h>

#include <algorithm>
#include <assert.h>

namespace onep
{
  void CountryNode::Definition::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .deriveClass<CountryNode, LuaVisualOsgNode<osg::Geode>>("CountryNode")
      .addFunction("get_country_name", &CountryNode::getCountryName)
      .addFunction("get_neighbours", &CountryNode::luaGetNeighbours)
      .endClass();
  }

  struct CountryNode::Impl
  {
    Impl() {}

    std::string name;

    osg::Vec2f centerLatLong;
    osg::Vec2f size;

    float earthRadius;
    float cameraZoom;

    List neighbors;
    BorderIdMap neighbourBorders;

    osg::ref_ptr<osg::Uniform> uniformColor;
    osg::ref_ptr<osg::Uniform> uniformAlpha;

    LuaRefPtr refNeighbours;
  };

  CountryNode::CountryNode(
    const LuaConfig::Ptr& configManager,
    const LuaStateManager::Ptr& lua,
    const std::string& countryName,
    osg::Vec2f centerLatLong,
    osg::Vec2f size,
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::Vec2Array> texcoords1,
    osg::ref_ptr<osg::Vec3Array> texcoords2,
    osg::ref_ptr<osg::DrawElementsUInt> triangles,
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

    m->uniformAlpha = new osg::Uniform("alpha", 0.0f);
    m->uniformColor = new osg::Uniform("overlayColor", osg::Vec3f(0.3f, 0.3f, 0.3f));

    addStateSetUniform(m->uniformAlpha);
    addStateSetUniform(m->uniformColor);
    addStateSetUniform(new osg::Uniform("overlayBlendColor", osg::Vec3f(1.0f, 0.0f, 0.0f)));
    addStateSetUniform(new osg::Uniform("takeover", 0.0f));
    addStateSetUniform(new osg::Uniform("takeoverColor", osg::Vec4f(0.0f, 0.0f, 1.0f, 0.8f)));
    addStateSetUniform(new osg::Uniform("takeoverScale", 100.0f));

    m->refNeighbours = MAKE_LUAREF_PTR(lua->newTable());
  }

  CountryNode::~CountryNode()
  {
  }

  void CountryNode::addNeighbor(osg::ref_ptr<CountryNode> mesh)
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
    BorderIdMap::iterator it = m->neighbourBorders.find(neighborId);
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

  void CountryNode::setColorMode(ColorMode mode)
  {
    switch (mode)
    {
    case MODE_SELECTED:
      m->uniformAlpha->set(0.5f);
      m->uniformColor->set(osg::Vec3f(0.5f, 0.69f, 1.0f));
      break;
    case MODE_NEIGHBOR:
      m->uniformAlpha->set(0.4f);
      m->uniformColor->set(osg::Vec3f(0.5f, 0.5f, 0.5f));
      break;
    case MODE_HIGHLIGHT_BANKS:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.0f, 0.0f, 0.8f));
      break;
    case MODE_HIGHLIGHT_CONTROL:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.635f, 0.439f, 0.031f)); // #A27008
      break;
    case MODE_HIGHLIGHT_CONCERNS:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.118f, 0.753f, 0.208f)); // #1EC035
      break;
    case MODE_HIGHLIGHT_MEDIA:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.902f, 1.0f, 0.357f)); // #E6FF5B
      break;
    case MODE_HIGHLIGHT_POLITICS:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.69f, 0.247f, 0.624f));
      break;
    default:
      assert(false);
      break;
    }
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