#include "scripting/LuaSkill.h"

#include <QtOsgBridge/Multithreading.h>

namespace onep
{
  void LuaSkill::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .deriveClass<LuaSkill, LuaCallbackRegistry>("LuaSkill")
      .addProperty("activated", &LuaSkill::getIsActivated, &LuaSkill::setIsActivated)
      .addProperty("name", &LuaSkill::getName)
      .addProperty("branch", &LuaSkill::getBranchName)
      .addFunction("update", &LuaSkill::update)
      .endClass();
  }

  struct LuaSkill::Impl
  {
    Impl()
      : cost(0)
      , obActivated(new osgHelper::Observable<bool>(false))
    {}

    std::string              name;
    std::string              branch;
    std::string              displayName;
    std::string              type;
    int                      cost;
    std::vector<std::string> dependencies;

    osgHelper::Observable<bool>::Ptr obActivated;
  };

  LuaSkill::LuaSkill(const luabridge::LuaRef& object, lua_State* lua, const LuaStateManager::Ptr& luaStateManager)
    : LuaCallbackRegistry(luaStateManager)
    , LuaTableMappedObject(object, lua)
    , m(new Impl())
  {
    m->name        = getString("name");
    m->branch      = getString("branch");
    m->displayName = getString("display_name");
    m->type        = getString("type");
    m->cost        = getNumber<int>("cost");

    if (hasValue("dependencies"))
    {
      const auto depTable = getTable("dependencies");

      depTable->iterateValues([this](const luabridge::Iterator& it)
      {
        checkType(it.value(), LUA_TSTRING);
        m->dependencies.push_back(it.value());
      });
    }

    auto bActivared = false;
    object["activated"] = bActivared;

    m->obActivated->set(bActivared);

    registerLuaCallback(LuaDefines::Callback::ON_SKILL_UPDATE);
  }

  LuaSkill::~LuaSkill() = default;

  void LuaSkill::update(const std::string& branchName, luabridge::LuaRef countryState)
  {
    triggerLuaCallback(LuaDefines::Callback::ON_SKILL_UPDATE, branchName, countryState);
  }

  const std::string& LuaSkill::getName() const
  {
    return m->name;
  }

  const std::string& LuaSkill::getBranchName() const
  {
    return m->branch;
  }

  const std::string& LuaSkill::getDisplayName() const
  {
    return m->displayName;
  }

  int LuaSkill::getCost() const
  {
    return m->cost;
  }

  bool LuaSkill::getIsActivated() const
  {
    return m->obActivated->get();
  }

  void LuaSkill::setIsActivated(bool activated)
  {
    QtOsgBridge::Multithreading::executeInUiAsync([=](){ m->obActivated->set(activated); });
  }

  osgHelper::Observable<bool>::Ptr LuaSkill::getObActivated() const
  {
    return m->obActivated;
  }

  const std::vector<std::string>& LuaSkill::getDependencies() const
  {
    return m->dependencies;
  }
}
