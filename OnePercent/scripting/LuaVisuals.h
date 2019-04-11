#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaVisuals : public osg::Referenced
  {
  public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    using Ptr = osg::ref_ptr<LuaVisuals>;

    LuaVisuals(osgGaming::Injector& injector);
    ~LuaVisuals();

    void updateBindings();

    void luaBindValueToVisuals(const std::string& value, const std::string& visual);
    void luaBindBranchValueToVisuals(const std::string& branchName, const std::string& branchValue, const std::string& visual);

    void luaUnbindValue(const std::string& value);
    void luaUnbindBranchValue(const std::string& branchName, const std::string& branchValue);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}