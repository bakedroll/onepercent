#include "scripting/LuaVisuals.h"

#include "core/Multithreading.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "scripting/LuaValueDef.h"
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

    bool isValueExisting(const std::string& name) const
    {
      auto valuesTable = modelContainer->getModel()->getValuesDefTable();
      return valuesTable->containsMappedElement(name);
    }

    bool isBranchExisting(const std::string& name) const
    {
      return modelContainer->getModel()->getBranchesTable()->containsMappedElement(name);
    }

    using VisualBindingsMap = std::map<std::string, std::string>;
    using PrototypeMap      = std::map<std::string, osg::ref_ptr<PrototypeNode>>;

    ModelContainer::Ptr  modelContainer;
    CountryOverlay::Ptr  countryOverlay;
    VisualBindingsMap    valueBindings;
    LuaStateManager::Ptr lua;

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
        }
      });
    });
  }

  void LuaVisuals::luaBindValueToVisuals(const std::string& value, const std::string& visual)
  {
    if (!m->isValueExisting(value))
    {
      OSGG_QLOG_WARN(QString("Visuals binding: Value '%1' does not exist.").arg(value.c_str()));
      assert_return(false);
    }

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
