#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaControl : public osg::Referenced, public LuaClassInstance
  {
  public:
    LuaControl(osgGaming::Injector& injector);
    ~LuaControl();

    virtual void registerClass(lua_State* state) override;

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