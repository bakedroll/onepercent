#include "SkillsContainer.h"
#include "SkillBranch.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  struct SkillsContainer::Impl
  {
    Impl() {}

    SkillBranch::List branches;

    std::map<std::string, int> nameIndexMap;
  };

  SkillsContainer::SkillsContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  SkillsContainer::~SkillsContainer()
  {
  }

  void SkillsContainer::addSkillBranch(SkillBranch::Ptr branch)
  {
    m->branches.push_back(branch);
    m->nameIndexMap[branch->getBranchName()] = int(m->branches.size()) - 1;
  }

  int SkillsContainer::getNumBranches()
  {
    return int(m->branches.size());
  }

  SkillBranch::Ptr SkillsContainer::getBranchByIndex(int i)
  {
    return m->branches[i];
  }

  SkillBranch::Ptr SkillsContainer::getBranchByName(std::string name)
  {
    return m->branches[m->nameIndexMap[name]];
  }

  void SkillsContainer::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<SkillsContainer>("SkillsContainer")
      .addFunction("add_branches", &SkillsContainer::lua_add_branches)
      .addFunction("add_skills", &SkillsContainer::lua_add_skills)
      .endClass();
  }

  std::string SkillsContainer::instanceVariableName()
  {
    return "skillsContainer";
  }

  void SkillsContainer::lua_add_branches(lua_State* state)
  {
    luaL_checktype(state, -1, LUA_TTABLE);

    lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
      luaL_checktype(state, -1, LUA_TTABLE);
      lua_getfield(state, -1, "name");
      lua_getfield(state, -2, "cost");

      const char* name = luaL_checkstring(state, -2);
      int cost = luaL_checkinteger(state, -1);

      OSGG_LOG_INFO("Branch added: " + std::string(name));

      addSkillBranch(new SkillBranch(int(m->branches.size()), std::string(name), cost));

      lua_pop(state, 3);
    }
  }

  void SkillsContainer::lua_add_skills(lua_State* state)
  {
    luaL_checktype(state, -1, LUA_TTABLE);

    lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
      luaL_checktype(state, -1, LUA_TTABLE);
      lua_getfield(state, -1, "name");
      lua_getfield(state, -2, "displayName");
      lua_getfield(state, -3, "type");
      lua_getfield(state, -4, "branch");
      lua_getfield(state, -5, "cost");

      const char* name = luaL_checkstring(state, -5);
      const char* displayName = luaL_checkstring(state, -4);
      const char* type = luaL_checkstring(state, -3);
      const char* branch = luaL_checkstring(state, -2);
      int cost = luaL_checkinteger(state, -1);

      if (m->nameIndexMap.count(branch) == 0)
      {
        OSGG_LOG_WARN("Branch " + std::string(name) + " does not exist");
      }
      else
      {
        m->branches[m->nameIndexMap[branch]]->addSkill(new Skill(std::string(name), std::string(displayName), std::string(type), cost));

        OSGG_LOG_INFO("Skill added: " + std::string(displayName));
      }

      lua_pop(state, 6);
    }
  }

}