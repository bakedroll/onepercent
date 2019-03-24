#include "scripting/LuaControl.h"

#include "nodes/CountryOverlay.h"
#include "scripting/LuaArrayTable.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSkillsTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaObservableCallback.h"
#include "simulation/ModelContainer.h"

#include <QString>

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
      .addFunction("skill", &LuaControl::luaGetSkill)
      .addFunction("update_branch", &LuaControl::luaUpdateBranch)
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

    osgGaming::Observer<LuaSimulationStateTable::CountryBranch>::Ptr branchActivatedObserver;
  };

  LuaControl::LuaControl(osgGaming::Injector& injector)
    : LuaCallbackRegistry(injector.inject<LuaStateManager>())
    , m(new Impl(injector))
  {
    registerLuaCallback(LuaDefines::Callback::ON_INITIALIZE);
    registerLuaCallback(LuaDefines::Callback::ON_TICK);
    registerLuaCallback(LuaDefines::Callback::ON_BRANCH_UPDATE);
    registerLuaCallback(LuaDefines::Callback::ON_BRANCH_PURCHASED);
    registerLuaCallback(LuaDefines::Callback::ON_BRANCH_ACTIVATED);
    registerLuaCallback(LuaDefines::Callback::ON_BRANCH_RESIGNED);
    
    registerLuaCallback(LuaDefines::Callback::ON_COUNTRY_CHANGED,
      std::make_shared<LuaObservableCallback<int>>(m->lua,
      injector.inject<CountryOverlay>()->getOSelectedCountryId()));

    registerLuaCallback(LuaDefines::Callback::ON_OVERLAY_CHANGED,
      std::make_shared<LuaObservableCallback<std::string>>(m->lua,
      injector.inject<CountryOverlay>()->getOCurrentOverlayBranchName()));

    const auto simState = m->modelContainer->getModel()->getSimulationStateTable();

    m->branchActivatedObserver = simState->getOCountryBranchActivated()->connect(
      osgGaming::Func<LuaSimulationStateTable::CountryBranch>([this](const LuaSimulationStateTable::CountryBranch& cb)
    {
      m->modelContainer->accessModel([this, &cb](const LuaModel::Ptr& model)
      {
        auto cstate = model->getSimulationStateTable()->getCountryState(cb.countryId);
        triggerLuaCallback(LuaDefines::Callback::ON_BRANCH_ACTIVATED, cb.branchName, cstate->luaref());
      });
    }));
  }

  void LuaControl::doSkillsUpdate(const LuaModel::Ptr& model)
  {
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

  void LuaControl::doBranchesUpdate(const LuaModel::Ptr& model)
  {
    auto states   = model->getSimulationStateTable();
    auto branches = model->getBranchesTable();

    states->foreachMappedElementDo<LuaCountryState>([this, &branches](LuaCountryState::Ptr& state)
    {
      auto countryState = state->luaref();

      branches->foreachMappedElementDo<LuaSkillBranch>([this, &countryState](LuaSkillBranch::Ptr& branch)
      {
        luaUpdateBranch(branch->getName(), countryState);
      });
    });
  }

  LuaSkill* LuaControl::luaGetSkill(const std::string& name) const
  {
    return m->modelContainer->getModel()->getBranchesTable()->findSkill(name).get();
  }

  void LuaControl::luaUpdateBranch(const std::string& name, luabridge::LuaRef countryState)
  {
    triggerLuaCallback(LuaDefines::Callback::ON_BRANCH_UPDATE, name, countryState);
  }

  void LuaControl::luaCreateBranches(luabridge::LuaRef branches)
  {
    assert_return(m->lua->checkIsType(branches, LUA_TTABLE));

    m->modelContainer->accessModel([&](const LuaModel::Ptr& model)
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

    m->modelContainer->accessModel([&](const LuaModel::Ptr& model)
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

        branch->getSkillsTable()->addSkill(m->lua, skillNameRef, skillRef);
      }
    });
  }

  void LuaControl::luaCreateCountries(luabridge::LuaRef countries)
  {
    assert_return(m->lua->checkIsType(countries, LUA_TTABLE));

    m->modelContainer->accessModel([&](const LuaModel::Ptr& model)
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

    m->modelContainer->accessModel([&](const LuaModel::Ptr& model)
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