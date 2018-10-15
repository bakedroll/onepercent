#include "scripting/LuaControl.h"

#include "scripting/LuaArrayTable.h"
#include "scripting/LuaMapTable.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "simulation/ModelContainer.h"

#include <QString>

namespace onep
{
  struct LuaControl::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
      , modelContainer(injector.inject<ModelContainer>())
    {
      luabridge::LuaRef refActions = lua->createGlobalTable("actions");

      initializeActionsTable    = lua->createElement<LuaArrayTable>("on_initialize",    refActions);
      tickActionsTable          = lua->createElement<LuaArrayTable>("on_tick",          refActions);
      skillActionsTable         = lua->createElement<LuaMapTable>  ("on_skill_update",  refActions);
      branchActionsTable        = lua->createElement<LuaArrayTable>("on_branch_update", refActions);
    }

    void callLuaFunctions(LuaArrayTable::Ptr& table)
    {
      table->foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
      {
        assert_return(lua->checkIsType(value, LUA_TFUNCTION));
        value();
      });
    }

    LuaStateManager::Ptr lua;

    ModelContainer::Ptr modelContainer;

    LuaArrayTable::Ptr   initializeActionsTable;
    LuaMapTable::Ptr     skillActionsTable;
    LuaArrayTable::Ptr   branchActionsTable;
    LuaArrayTable::Ptr   tickActionsTable;
  };

  LuaControl::LuaControl(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance("control")
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
      .addFunction("create_branches",         &LuaControl::luaCreateBranches)
      .addFunction("create_skills",           &LuaControl::luaCreateSkills)
      .addFunction("create_countries",        &LuaControl::luaCreateCountries)
      .addFunction("create_values",           &LuaControl::luaCreateValues)
      .endClass();
  }

  void LuaControl::triggerOnInitializeEvents()
  {
    m->callLuaFunctions(m->initializeActionsTable);
  }

  void LuaControl::triggerOnTickActions()
  {
    m->callLuaFunctions(m->tickActionsTable);
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

  void LuaControl::luaCreateBranches(luabridge::LuaRef branches)
  {
    assert_return(m->lua->checkIsType(branches, LUA_TTABLE));

    m->modelContainer->accessModel([&](std::shared_ptr<LuaModel> model)
    {
      for (luabridge::Iterator it(branches); !it.isNil(); ++it)
      {
        auto branchRef = it.value();
        assert_continue(m->lua->checkIsType(branchRef, LUA_TTABLE));
        
        auto nameRef = branchRef["name"];
        assert_continue(m->lua->checkIsType(nameRef, LUA_TSTRING));

        model->getBranchesTable()->addBranch(nameRef, branchRef);
      }
    });
  }

  void LuaControl::luaCreateSkills(luabridge::LuaRef skills)
  {
    assert_return(m->lua->checkIsType(skills, LUA_TTABLE));

    m->modelContainer->accessModel([&](std::shared_ptr<LuaModel> model)
    {
      for (luabridge::Iterator it(skills); !it.isNil(); ++it)
      {
        auto skillRef = it.value();
        assert_continue(m->lua->checkIsType(skillRef, LUA_TTABLE));

        auto branchNameRef = skillRef["branch"];
        assert_continue(m->lua->checkIsType(branchNameRef, LUA_TSTRING));

        auto skillNameRef = skillRef["name"];
        assert_continue(m->lua->checkIsType(skillNameRef, LUA_TSTRING));

        auto branch = model->getBranchesTable()->getBranchByName(branchNameRef);
        if (!branch)
        {
          OSGG_QLOG_WARN(QString("Cannot add skill '%1'. Branch '%2' not found")
            .arg(QString::fromStdString(skillNameRef))
            .arg(QString::fromStdString(branchNameRef)));
          continue;
        }

        branch->getSkillsTable()->addSkill(skillNameRef, skillRef);
      }
    });
  }

  void LuaControl::luaCreateCountries(luabridge::LuaRef countries)
  {
    assert_return(m->lua->checkIsType(countries, LUA_TTABLE));

    m->modelContainer->accessModel([&](std::shared_ptr<LuaModel> model)
    {
      for (luabridge::Iterator it(countries); !it.isNil(); ++it)
      {
        auto countryRef = it.value();
        assert_continue(m->lua->checkIsType(countryRef, LUA_TTABLE));

        auto idRef = countryRef["id"];
        assert_continue(m->lua->checkIsType(idRef, LUA_TNUMBER));

        model->getCountriesTable()->addCountry(idRef, countryRef);
      }
    });
  }

  void LuaControl::luaCreateValues(luabridge::LuaRef values)
  {
    assert_return(m->lua->checkIsType(values, LUA_TTABLE));

    m->modelContainer->accessModel([&](std::shared_ptr<LuaModel> model)
    {
      for (luabridge::Iterator it(values); !it.isNil(); ++it)
      {
        auto valueRef = it.value();
        assert_continue(m->lua->checkIsType(valueRef, LUA_TTABLE));

        auto nameRef = valueRef["name"];
        assert_continue(m->lua->checkIsType(nameRef, LUA_TSTRING));

        model->getValuesDefTable()->addValue(nameRef, valueRef);
      }
    });
  }
}