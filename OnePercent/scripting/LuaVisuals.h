#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaVisuals : public osg::Referenced
  {
  public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

    using Ptr = osg::ref_ptr<LuaVisuals>;

    LuaVisuals(osgGaming::Injector& injector);
    ~LuaVisuals();

    void updateVisualBindings();

    void luaBindValueToVisuals(const std::string& value, const std::string& visual);
    void luaBindBranchValueToVisuals(const std::string& branchName, const std::string& branchValue, const std::string& visual);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}