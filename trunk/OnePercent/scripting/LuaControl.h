#pragma once

#include "scripting/LuaStateManager.h"
#include "scripting/LuaCallbackRegistry.h"

namespace onep
{
  class LuaControl : public osg::Referenced, public LuaCallbackRegistry, public LuaClassInstance
  {
  public:
    LuaControl(osgGaming::Injector& injector);

    virtual void registerClass(lua_State* state) override;

    void doSkillsUpdate();
    void doBranchesUpdate();

    void triggerOnInitializeEvents();
    void triggerOnTickActions();

    void luaOnInitializeAction(luabridge::LuaRef func);
    void luaOnTickAction(luabridge::LuaRef func);
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