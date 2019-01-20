#pragma once

#include "scripting/LuaStateManager.h"
#include "scripting/LuaCallbackRegistry.h"

namespace onep
{
  class LuaSkill;

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

    LuaSkill* getSkill(const std::string& name);

    void updateBranch(const std::string& name, luabridge::LuaRef countryState);

    void luaCreateBranches(luabridge::LuaRef branches);
    void luaCreateSkills(luabridge::LuaRef skills);
    void luaCreateCountries(luabridge::LuaRef countries);
    void luaCreateValues(luabridge::LuaRef values);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}