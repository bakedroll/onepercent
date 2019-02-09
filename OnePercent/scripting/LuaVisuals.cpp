#include "scripting/LuaVisuals.h"

#include "core/Multithreading.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "scripting/LuaValueDef.h"
#include "simulation/ModelContainer.h"
#include "nodes/CountryOverlay.h"

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
    }

    bool isValueOfTypeExisting(const std::string& name, LuaValueDef::Type type) const
    {
      auto valuesTable = modelContainer->getModel()->getValuesDefTable();

      return (valuesTable->containsMappedElement(name) && 
        (valuesTable->getMappedElement<LuaValueDef>(name)->getType() == type));
    }

    bool isBranchExisting(const std::string& name) const
    {
      return modelContainer->getModel()->getBranchesTable()->containsMappedElement(name);
    }

    template <typename ValueType>
    void setUniform(const CountryNode::Ptr& node, const std::string& visual, const ValueType& value)
    {
      auto uniform = node->getStateSetUniform(visual);
      assert_return(uniform);

      if (!uniform)
      {
        OSGG_QLOG_WARN(QString("Uniform %1 not found.").arg(visual.c_str()));
      }

      uniform->set(value);
    }

    using VisualBindingsMap = std::map<std::string, std::string>;
    using BranchBindingsMap = std::map<std::string, VisualBindingsMap>;

    ModelContainer::Ptr modelContainer;
    CountryOverlay::Ptr countryOverlay;
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
          const auto node = m->countryOverlay->getCountryNode(state.first);

          auto branchValuesTable = state.second->getBranchValuesTable();
          auto valuesTable = state.second->getValuesTable();

          for (auto& visual : m->valueBindings)
          {
            m->setUniform(node, visual.second, valuesTable->getValue(visual.first));
          }

          for (auto& branchBindings : m->branchValueBindings)
          {
            auto  branch = branchValuesTable->getBranch(branchBindings.first);
            for (auto& visual : branchBindings.second)
            {
              m->setUniform(node, visual.second, branch->getValue(visual.first));
            }
          }
        }
      });
    });
  }

  void LuaVisuals::luaBindValueToVisuals(const std::string& value, const std::string& visual)
  {
    if (!m->isValueOfTypeExisting(value, LuaValueDef::Type::Default))
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
    if (!(m->isValueOfTypeExisting(branchValue, LuaValueDef::Type::Branch) && m->isBranchExisting(branchName)))
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
