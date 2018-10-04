#include "scripting/LuaSkillBranch.h"
#include "scripting/LuaSkillsTable.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaSkillBranch::Impl
  {
    Impl()
      : cost(0)
    {}

    std::string name;
    int cost;

    LuaSkillsTable::Ptr skillsTable;
  };

  LuaSkillBranch::LuaSkillBranch(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());

    auto nameRef = object["name"];
    auto costRef = object["cost"];

    assert_return(nameRef.isString());
    assert_return(costRef.isNumber());

    std::string name = nameRef;

    m->name = name;
    m->cost = costRef;

    m->skillsTable = makeMappedElement<LuaSkillsTable>("skills");
  }

  LuaSkillBranch::~LuaSkillBranch() = default;

  std::shared_ptr<LuaSkillsTable> LuaSkillBranch::getSkillsTable() const
  {
    return m->skillsTable;
  }

  const std::string& LuaSkillBranch::getBranchName() const
  {
    return m->name;
  }

  int LuaSkillBranch::getCost() const
  {
    return m->cost;
  }
}