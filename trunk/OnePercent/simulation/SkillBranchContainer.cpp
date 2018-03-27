#include "SkillBranchContainer.h"
#include "SkillBranch.h"

namespace onep
{
  struct SkillBranchContainer::Impl
  {
    Impl() {}

    std::vector<SkillBranch::Ptr> branches;
    std::map<std::string, int> nameIndexMap;
  };

  SkillBranchContainer::SkillBranchContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  SkillBranchContainer::~SkillBranchContainer()
  {
  }

  void SkillBranchContainer::addSkillBranch(SkillBranch::Ptr branch)
  {
    m->branches.push_back(branch);
    m->nameIndexMap[branch->getBranchName()] = int(m->branches.size()) - 1;
  }

  int SkillBranchContainer::getNumBranches()
  {
    return int(m->branches.size());
  }

  SkillBranch::Ptr SkillBranchContainer::getBranchByIndex(int i)
  {
    return m->branches[i];
  }

  SkillBranch::Ptr SkillBranchContainer::getBranchByName(std::string name)
  {
    return m->branches[m->nameIndexMap[name]];
  }

  void SkillBranchContainer::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<SkillBranchContainer>("SkillBranchContainer")
      .addFunction("add_branches", &SkillBranchContainer::lua_add_branches)
      .endClass();
  }

  std::string SkillBranchContainer::instanceVariableName()
  {
    return "skillBranchContainer";
  }

  void SkillBranchContainer::lua_add_branches(lua_State* state)
  {
    luaL_checktype(state, -1, LUA_TTABLE);

    lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
      luaL_checktype(state, -1, LUA_TTABLE);
      lua_getfield(state, -1, "name");
      lua_getfield(state, -2, "cost");

      const char* name = luaL_checkstring(state, -2);
      int costs = luaL_checkinteger(state, -1);

      OSGG_LOG_INFO("Branch: " + std::string(name));

      addSkillBranch(new SkillBranch(int(m->branches.size()), std::string(name), costs));

      lua_pop(state, 3);
    }
  }
}