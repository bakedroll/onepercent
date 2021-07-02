#pragma once

#include <memory>
#include <vector>

#include <luaHelper/LuaTableMappedObject.h>

namespace onep
{
  class LuaSkillsTable;
  class LuaSkill;

  class LuaSkillBranch : public luaHelper::LuaTableMappedObject
  {
  public:
    typedef std::shared_ptr<LuaSkillBranch> Ptr;
    typedef std::vector<Ptr>                List;
    typedef std::map<std::string, Ptr>      Map;

    explicit LuaSkillBranch(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSkillBranch();

    std::shared_ptr<LuaSkillsTable> getSkillsTable() const;
    const std::string&              getName() const;
    int                             getCost() const;

  private:
    struct Impl;
    std::shared_ptr<Impl> m;
  };
}