#include "SkillBranch.h"
#include "simulation/LuaSkillsTable.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct SkillBranch::Impl
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

  SkillBranch::SkillBranch(const luabridge::LuaRef& object)
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

  SkillBranch::~SkillBranch() = default;

  std::shared_ptr<LuaSkillsTable> SkillBranch::getSkillsTable() const
  {
    return m->skillsTable;
  }

  int SkillBranch::getBranchId() const
  {
    return m->id;
  }

  const std::string& SkillBranch::getBranchName() const
  {
    return m->name;
  }

  int SkillBranch::getCost() const
  {
    return m->cost;
  }

  void SkillBranch::setBranchId(int id)
  {
    m->id = id;
  }
}