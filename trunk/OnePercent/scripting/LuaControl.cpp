#include "scripting/LuaControl.h"

#include "scripting/LuaArrayTable.h"
#include "scripting/LuaMapTable.h"

namespace onep
{
  struct LuaControl::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {
      luabridge::LuaRef refActions = lua->createGlobalTable("actions");

      initializeActionsTable    = lua->createElement<LuaArrayTable>("on_initialize",    refActions);
      tickActionsTable          = lua->createElement<LuaArrayTable>("on_tick",          refActions);
      skillActionsTable         = lua->createElement<LuaMapTable>  ("on_skill_update",  refActions);
      branchActionsTable        = lua->createElement<LuaArrayTable>("on_branch_update", refActions);
    }

    LuaStateManager::Ptr lua;
    LuaArrayTable::Ptr   initializeActionsTable;
    LuaMapTable::Ptr     skillActionsTable;
    LuaArrayTable::Ptr   branchActionsTable;
    LuaArrayTable::Ptr   tickActionsTable;
  };

  LuaControl::LuaControl(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance()
    , m(new Impl(injector))
  {
  }

  LuaControl::~LuaControl() = default;

  void LuaControl::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaControl>("Control")
      .addFunction("on_initialize_action",    &LuaControl::luaOnInitializeAction)
      .addFunction("on_tick_action",          &LuaControl::luaOnTickAction)
      .addFunction("on_skill_update_action",  &LuaControl::luaOnSkillUpdateAction)
      .addFunction("on_branch_update_action", &LuaControl::luaOnBranchUpdateAction)
      .endClass();
  }

  std::string LuaControl::instanceVariableName()
  {
    return "control";
  }

  void LuaControl::luaOnInitializeAction(luabridge::LuaRef func)
  {
    m->lua->safeExecute([this, &func]()
    {
      assert_return(m->lua->checkIsType(func, LUA_TFUNCTION));
      m->initializeActionsTable->addEement(func);
    });
  }

  void LuaControl::luaOnTickAction(luabridge::LuaRef func)
  {
    m->lua->safeExecute([&]()
    {
      assert_return(m->lua->checkIsType(func, LUA_TFUNCTION));
      m->tickActionsTable->addEement(func);
    });
  }

  void LuaControl::luaOnSkillUpdateAction(const std::string& name, luabridge::LuaRef func)
  {
    m->lua->safeExecute([&]()
    {
      assert_return(m->lua->checkIsType(func, LUA_TFUNCTION));
      if (!m->skillActionsTable->contains(name))
      {
        m->skillActionsTable->insert(name, m->lua->newTable());
      }

      m->skillActionsTable->getElement(name).append(func);
    });
  }

  void LuaControl::luaOnBranchUpdateAction(luabridge::LuaRef func)
  {
    m->lua->safeExecute([&]()
    {
      assert_return(m->lua->checkIsType(func, LUA_TFUNCTION));
      m->branchActionsTable->addEement(func);
    });
  }
}