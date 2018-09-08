#include "scripting/LuaSkillBranch.h"
#include "scripting/LuaSkillsTable.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaSkillBranch::Impl
  {
    Impl()
      : id(-1)
      , cost(0)
    {}

    int id;
    std::string name;
    int cost;

    LuaSkillsTable::Ptr skillsTable;
  };

  LuaSkillBranch::LuaSkillBranch(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef nameRef = object["name"];
    luabridge::LuaRef costRef = object["cost"];

    assert_return(nameRef.isString());
    assert_return(costRef.isNumber());

    m->name = nameRef.tostring();
    m->cost = costRef;

    m->skillsTable = makeMappedElement<LuaSkillsTable>("skills");
  }

  LuaSkillBranch::~LuaSkillBranch() = default;

  std::shared_ptr<LuaSkillsTable> LuaSkillBranch::getSkillsTable() const
  {
    return m->skillsTable;
  }

  int LuaSkillBranch::getBranchId() const
  {
    return m->id;
  }

  const std::string& LuaSkillBranch::getBranchName() const
  {
    return m->name;
  }

  int LuaSkillBranch::getCost() const
  {
    return m->cost;
  }

  void LuaSkillBranch::setBranchId(int id)
  {
    m->id = id;
  }
}