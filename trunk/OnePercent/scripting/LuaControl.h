#pragma once

#include "scripting/LuaStateManager.h"
#include "scripting/LuaCallbackRegistry.h"

namespace onep
{
  class LuaControl : public LuaCallbackRegistry
  {
  public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

    LuaControl(osgGaming::Injector& injector);

    void doSkillsUpdate();
    void doBranchesUpdate();

    void luaOnSkillUpdateAction(const std::string& name, luabridge::LuaRef func);
    void luaOnBranchUpdateAction(luabridge::LuaRef func);

    void luaCreateBranches(luabridge::LuaRef branches);
    void luaCreateSkills(luabridge::LuaRef skills);
    void luaCreateCountries(luabridge::LuaRef countries);
    void luaCreateValues(luabridge::LuaRef values);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}