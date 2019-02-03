#include "core/Multithreading.h"
#include "scripting/LuaVisuals.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaSimulationStateTable.h"
#include "simulation/ModelContainer.h"
#include "nodes/CountryOverlay.h"

#include <osg/Uniform>

#include <QString>

namespace onep
{
  void LuaVisuals::Definition::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaVisuals>("Visuals")
      .addFunction("bind_value_to_visuals", &LuaVisuals::luaBindValueToVisuals)
      .addFunction("bind_branch_value_to_visuals", &LuaVisuals::luaBindBranchValueToVisuals)
      .endClass();
  }

  struct LuaVisuals::Impl
  {
    Impl(osgGaming::Injector& injector)
      : modelContainer(injector.inject<ModelContainer>())
      , countryOverlay(injector.inject<CountryOverlay>())
    {
      initializeUniformGetters();
    }

    void initializeUniformGetters()
    {
      uniformGetters["takeover"] = [this](int cid) { return countryOverlay->getCountryNode(cid)->getTakeoverUniform(); };
    }

    LuaCountryState::Ptr getFirstCountryState() const
    {
      auto& cstates = modelContainer->getModel()->getSimulationStateTable()->getCountryStates();

      auto cstateIt = cstates.begin();
      if (cstateIt == std::end(cstates))
      {
        return LuaCountryState::Ptr();
      }

      return cstateIt->second;
    }

    bool isValueExisting(const std::string& value) const
    {
      auto cstate = getFirstCountryState();
      if (!cstate)
      {
        return false;
      }

      return cstate->getValuesTable()->contains(value);
    }

    bool isBranchValueExisting(const std::string& branchName, const std::string& valueName) const
    {
      auto cstate = getFirstCountryState();
      if (!cstate)
      {
        return false;
      }

      auto branchValuesTable = cstate->getBranchValuesTable();
      if (!cstate->getBranchValuesTable()->containsMappedElement(branchName))
      {
        return false;
      }

      return branchValuesTable->getBranch(branchName)->contains(valueName);
    }

    template <typename T>
    void setUniform(int cid, const std::string& visual, const T& value)
    {
      auto& getter = uniformGetters[visual];
      auto  uniform = getter(cid);

      uniform->set(value);
    }

    using UniformGetter     = std::function<osg::ref_ptr<osg::Uniform>(int cid)>;
    using UniformGetterMap  = std::map<std::string, UniformGetter>;
    using VisualBindingsMap = std::map<std::string, std::string>;
    using BranchBindingsMap = std::map<std::string, VisualBindingsMap>;

    ModelContainer::Ptr modelContainer;
    CountryOverlay::Ptr countryOverlay;
    UniformGetterMap    uniformGetters;
    VisualBindingsMap   valueBindings;
    BranchBindingsMap   branchValueBindings;
  };

  LuaVisuals::LuaVisuals(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
  }

  LuaVisuals::~LuaVisuals()
  {
  }

  void LuaVisuals::updateVisualBindings()
  {
    Multithreading::uiExecuteAsync([this]()
    {
      m->modelContainer->accessModel([this](const LuaModel::Ptr& model)
      {
        auto& states = model->getSimulationStateTable()->getCountryStates();
        for (auto& state : states)
        {
          auto branchValuesTable = state.second->getBranchValuesTable();
          auto valuesTable = state.second->getValuesTable();

          for (auto& visual : m->valueBindings)
          {
            m->setUniform(state.first, visual.second, valuesTable->getValue(visual.first));
          }

          for (auto& branchBindings : m->branchValueBindings)
          {
            auto  branch = branchValuesTable->getBranch(branchBindings.first);
            for (auto& visual : branchBindings.second)
            {
              m->setUniform(state.first, visual.second, branch->getValue(visual.first));
            }
          }
        }
      });
    });
  }

  void LuaVisuals::luaBindValueToVisuals(const std::string& value, const std::string& visual)
  {
    if (!m->isValueExisting(value))
    {
      OSGG_QLOG_WARN(QString("Value '%1' does not exist.").arg(value.c_str()));
      return;
    }

    m->valueBindings[value] = visual;

    OSGG_QLOG_DEBUG(QString("Value visuals binding added: %1 -> %2")
      .arg(value.c_str())
      .arg(visual.c_str()));
  }

  void LuaVisuals::luaBindBranchValueToVisuals(const std::string& branchName, const std::string& branchValue, const std::string& visual)
  {
    if (!m->isBranchValueExisting(branchName, branchValue))
    {
      OSGG_QLOG_WARN(QString("Branch value '%1.%2' does not exist.").arg(branchName.c_str()).arg(branchValue.c_str()));
      return;
    }

    m->branchValueBindings[branchName][branchValue] = visual;

    OSGG_QLOG_DEBUG(QString("Branch value visuals binding added: %1.%2 -> %3")
      .arg(branchName.c_str())
      .arg(branchValue.c_str())
      .arg(visual.c_str()));
  }
}
