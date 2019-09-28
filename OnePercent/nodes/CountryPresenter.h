#pragma once

#include "scripting/LuaBridgeDefinition.h"
#include "scripting/LuaConfig.h"

#include <osg/Group>

namespace onep
{
  class CountryPresenter : public osg::Group
  {
  public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using Ptr = osg::ref_ptr<CountryPresenter>;
    using Map = std::map<int, Ptr>;

    CountryPresenter(const LuaConfig::Ptr& configManager, const osg::Vec2f& centerLatLong, const osg::Vec2f& size);

    osg::Vec2f getCenterLatLong() const;
    osg::Vec2f getSize() const;
    osg::Vec2f getSurfaceSize() const;
    float      getOptimalCameraDistance(float angle, float ratio) const;

    void luaAddNode(osg::Node* node);
    void luaClearNodes();

private:
    osg::Vec2f m_centerLatLong;
    osg::Vec2f m_size;
    float      m_earthRadius;
    float      m_cameraZoom;

  };
}
