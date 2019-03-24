#pragma once

#include "scripting/LuaStateManager.h"
#include "scripting/LuaCallbackRegistry.h"

namespace onep
{
  class LuaModel;
  class LuaSkill;

  class LuaControl : public LuaCallbackRegistry
  {
  public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

    using Ptr = osg::ref_ptr<LuaControl>;

    LuaControl(osgGaming::Injector& injector);

    void doSkillsUpdate(const std::shared_ptr<LuaModel>& model);
    void doBranchesUpdate(const std::shared_ptr<LuaModel>& model);

    LuaSkill* luaGetSkill(const std::string& name) const;

    void luaUpdateBranch(const std::string& name, luabridge::LuaRef countryState);

    void luaCreateBranches(luabridge::LuaRef branches);
    void luaCreateSkills(luabridge::LuaRef skills);
    void luaCreateCountries(luabridge::LuaRef countries);
    void luaCreateValues(luabridge::LuaRef values);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}