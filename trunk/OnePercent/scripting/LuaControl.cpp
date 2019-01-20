#include "scripting/LuaControl.h"

#include "scripting/LuaArrayTable.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "simulation/ModelContainer.h"

#include <QString>
#include "LuaCountryState.h"

namespace onep
{
  void LuaControl::Definition::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .deriveClass<LuaControl, LuaCallbackRegistry>("Control")
      .addFunction("create_branches", &LuaControl::luaCreateBranches)
      .addFunction("create_skills", &LuaControl::luaCreateSkills)
      .addFunction("create_countries", &LuaControl::luaCreateCountries)
      .addFunction("create_values", &LuaControl::luaCreateValues)
      .addFunction("skill", &LuaControl::getSkill)
      .addFunction("update_branch", &LuaControl::updateBranch)
      .endClass();
  }

  struct LuaControl::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
      , modelContainer(injector.inject<ModelContainer>())
    {
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
  };

  LuaControl::LuaControl(osgGaming::Injector& injector)
    : LuaCallbackRegistry()
    , m(new Impl(injector))
  {
    registerLuaCallback(LuaDefines::Callback::ON_INITIALIZE);
    registerLuaCallback(LuaDefines::Callback::ON_TICK);
    registerLuaCallback(LuaDefines::Callback::ON_BRANCH_UPDATE);
  }

  void LuaControl::doSkillsUpdate()
  {
    auto model    = m->modelContainer->getModel();
    auto states   = model->getSimulationStateTable();
    auto branches = model->getBranchesTable();

    states->foreachMappedElementDo<LuaCountryState>([this, &branches](LuaCountryState::Ptr& state)
    {
      auto branchesActivated = state->getBranchesActivatedTable();
      auto countryState      = state->luaref();

      branches->foreachMappedElementDo<LuaSkillBranch>([this, &branchesActivated, &countryState](LuaSkillBranch::Ptr& branch)
      {
        auto branchName = branch->getName();
        if (!branchesActivated->getBranchActivated(branchName))
        {
          return;
        }

        auto skills = branch->getSkillsTable();
        for (auto& skill : skills->getSkillsMap())
        {
          auto skillName = skill.second->getName();
          if (!skill.second->getIsActivated())
          {
            return;
          }

          skill.second->update(branchName, countryState);
        }
      });
    });
  }

  void LuaControl::doBranchesUpdate()
  {
    auto model    = m->modelContainer->getModel();
    auto states   = model->getSimulationStateTable();
    auto branches = model->getBranchesTable();

    states->foreachMappedElementDo<LuaCountryState>([this, &branches](LuaCountryState::Ptr& state)
    {
      auto countryState = state->luaref();

      branches->foreachMappedElementDo<LuaSkillBranch>([this, &countryState](LuaSkillBranch::Ptr& branch)
      {
        updateBranch(branch->getName(), countryState);
      });
    });
  }

  LuaSkill* LuaControl::getSkill(const std::string& name)
  {
    return m->modelContainer->getModel()->getBranchesTable()->findSkill(name).get();
  }

  void LuaControl::updateBranch(const std::string& name, luabridge::LuaRef countryState)
  {
    triggerLuaCallback(LuaDefines::Callback::ON_BRANCH_UPDATE, name, countryState);
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