#include "scripting/LuaVisuals.h"

#include "core/Multithreading.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaModelPrototype.h"
#include "simulation/ModelContainer.h"
#include "nodes/CountryOverlay.h"

#include <osgGaming/Helper.h>

#include <osg/PositionAttitudeTransform>

#include <osgDB/ReadFile>

namespace onep
{
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

  void LuaVisuals::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .beginClass<LuaVisuals>("Visuals")
      .addFunction("bind_value_to_visuals", &LuaVisuals::luaBindValueToVisuals)
      .addFunction("unbind_value", &LuaVisuals::luaUnbindValue)
      .addFunction("bind_group_value_to_visuals", &LuaVisuals::luaBindGroupValueToVisuals)
      .addFunction("unbind_group_value", &LuaVisuals::luaUnbindGroupValue)
      .addFunction("update_bindings", &LuaVisuals::updateBindings)
      .addFunction("register_model_prototype", &LuaVisuals::luaRegisterModelPrototype)
      .addFunction("get_model_prototype", &LuaVisuals::luaGetModelPrototype)
      .addFunction("create_text_node", &LuaVisuals::luaCreateTextNode)
      .endClass();
  }

  struct LuaVisuals::Impl
  {
    explicit Impl(osgGaming::Injector& injector)
      : modelContainer(injector.inject<ModelContainer>())
      , countryOverlay(injector.inject<CountryOverlay>())
      , lua(injector.inject<LuaStateManager>())
    {
    }

    using VisualBindingsMap      = std::map<std::string, std::string>;
    using VisualGroupBindingsMap = std::map<std::string, VisualBindingsMap>;
    using PrototypeMap           = std::map<std::string, osg::ref_ptr<PrototypeNode>>;

    ModelContainer::Ptr    modelContainer;
    CountryOverlay::Ptr    countryOverlay;
    VisualBindingsMap      valueBindings;
    VisualGroupBindingsMap groupValueBindings;
    LuaStateManager::Ptr   lua;

    PrototypeMap prototypes;
  };

  LuaVisuals::LuaVisuals(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
  }

  LuaVisuals::~LuaVisuals() = default;

  void LuaVisuals::updateBindings()
  {
    Multithreading::executeInUiAsync([this]()
    {
      m->modelContainer->accessModel([this](const LuaModel::Ptr& model)
      {
        auto& states = model->getSimulationStateTable()->getCountryStates();
        for (auto& state : states)
        {
          const auto node        = m->countryOverlay->getCountryNode(state.first);
          auto       valuesTable = state.second->getValuesTable();

          for (auto& visual : m->valueBindings)
          {
            setUniform(node, visual.second, valuesTable->getValue(visual.first));
          }

          for (const auto& group : m->groupValueBindings)
          {
            const auto valueGroup = valuesTable->getGroup(group.first);
            assert_continue(valueGroup);

            for (const auto& visual : group.second)
            {
              setUniform(node, visual.second, valueGroup->getValue(visual.first));
            }
          }
        }
      });
    });
  }

  void LuaVisuals::luaBindValueToVisuals(const std::string& value, const std::string& visual)
  {
    m->valueBindings[value] = visual;

    OSGG_QLOG_DEBUG(QString("Value visuals binding added: %1 -> %2")
      .arg(value.c_str())
      .arg(visual.c_str()));
  }

  void LuaVisuals::luaUnbindValue(const std::string& value)
  {
    auto it = m->valueBindings.find(value);
    if (it == m->valueBindings.end())
    {
      OSGG_QLOG_WARN(QString("Visuals unbinding: Binding for value '%1' does not exist").arg(value.c_str()));
      assert_return(false);
    }

    m->valueBindings.erase(it);
  }

  void LuaVisuals::luaBindGroupValueToVisuals(const std::string& group, const std::string& value,
    const std::string& visual)
  {
      m->groupValueBindings[group][value] = visual;

      OSGG_QLOG_DEBUG(QString("Group value visuals binding added: %1.%2 -> %3")
          .arg(group.c_str())
          .arg(value.c_str())
          .arg(visual.c_str()));
  }

  void LuaVisuals::luaUnbindGroupValue(const std::string& group, const std::string& value)
  {
      auto groupIt = m->groupValueBindings.find(group);
      if (groupIt == m->groupValueBindings.end())
      {
        OSGG_QLOG_WARN(QString("Visuals unbinding: Binding for group '%1' does not exist").arg(group.c_str()));
        assert_return(false);
      }

      auto it = groupIt->second.find(value);
      if (it == groupIt->second.end())
      {
        OSGG_QLOG_WARN(QString("Visuals unbinding: Binding for group value '%1.%2' does not exist")
                               .arg(group.c_str())
                               .arg(value.c_str()));
        assert_return(false);
      }

      groupIt->second.erase(it);
      if (groupIt->second.empty())
      {
        m->groupValueBindings.erase(groupIt);
      }
  }

  void LuaVisuals::luaRegisterModelPrototype(const std::string& prototypeName, luabridge::LuaRef table)
  {
    auto prototype = m->lua->makeElementFromTable<LuaModelPrototype>(table);
    auto filename  = prototype->getFilename();

    if (filename.empty())
    {
      return;
    }

    auto node     = osgDB::readNodeFile(filename);
    auto rotation = prototype->getRotationDeg();
    auto degToRad = 180.0f / C_PI;

    osg::ref_ptr<PrototypeNode> transform = new PrototypeNode();
    transform->setScale(prototype->getScale());
    transform->setAttitude(osgGaming::getQuatFromEuler(rotation.x() / degToRad, rotation.y() / degToRad, rotation.z() / degToRad));
    transform->setPosition(prototype->getPosition());

    transform->addChild(node);

    m->prototypes[prototypeName] = transform;
  }

  LuaVisuals::PrototypeNode* LuaVisuals::luaGetModelPrototype(const std::string& prototypeName) const
  {
    if (m->prototypes.count(prototypeName) == 0)
    {
      OSGG_QLOG_WARN(QString("Model prototype '%1' not registered.").arg(QString::fromStdString(prototypeName)));
      return nullptr;
    }

    return m->prototypes.find(prototypeName)->second;
  }

  TextNode* LuaVisuals::luaCreateTextNode(const std::string& text) const
  {
    return new TextNode(text);
  }
}
