#pragma once

#include "nodes/TextNode.h"

#include <osg/PositionAttitudeTransform>

#include <luaHelper/LuaStateManager.h>

namespace onep
{
  class LuaVisuals : public osg::Referenced
  {
  public:
    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using Ptr           = osg::ref_ptr<LuaVisuals>;
    using PrototypeNode = LuaVisualOsgNode<osg::PositionAttitudeTransform>;

    LuaVisuals(osgHelper::ioc::Injector& injector);
    ~LuaVisuals();

    void updateBindings();

    void luaBindValueToVisuals(const std::string& value, const std::string& visual);
    void luaUnbindValue(const std::string& value);

    void luaBindGroupValueToVisuals(const std::string& group, const std::string& value, const std::string& visual);
    void luaUnbindGroupValue(const std::string& group, const std::string& value);

    void luaRegisterModelPrototype(const std::string& prototypeName, luabridge::LuaRef table);
    PrototypeNode* luaGetModelPrototype(const std::string& prototypeName) const;

    TextNode* luaCreateTextNode(const std::string& text) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
