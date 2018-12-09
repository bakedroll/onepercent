#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaVisuals : public osg::Referenced, public LuaClassInstance
  {
  public:
    using Ptr = osg::ref_ptr<LuaVisuals>;

    LuaVisuals(osgGaming::Injector& injector);
    ~LuaVisuals();

    virtual void registerClass(lua_State* state) override;

    void updateVisualBindings();

    void luaBindValueToVisuals(const std::string& value, const std::string& visual);
    void luaBindBranchValueToVisuals(const std::string& branchName, const std::string& branchValue, const std::string& visual);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}