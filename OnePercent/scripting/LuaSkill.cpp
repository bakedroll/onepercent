#include "scripting/LuaSkill.h"
#include "core/Multithreading.h"

#include <osgGaming/Macros.h>

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
      , obActivated(new osgGaming::Observable<bool>(false))
    {}

    std::string              name;
    std::string              branch;
    std::string              displayName;
    std::string              type;
    int                      cost;
    std::vector<std::string> dependencies;

    osgGaming::Observable<bool>::Ptr obActivated;
  };

  LuaSkill::LuaSkill(const luabridge::LuaRef& object, lua_State* lua, const LuaStateManager::Ptr& luaStateManager)
    : LuaCallbackRegistry(luaStateManager)
    , LuaObjectMapper(object, lua)
    , m(new Impl())
  {
    checkForConsistency("name", LUA_TSTRING);
    checkForConsistency("branch", LUA_TSTRING);
    checkForConsistency("display_name", LUA_TSTRING);
    checkForConsistency("type", LUA_TSTRING);
    checkForConsistency("cost", LUA_TNUMBER);

    assert_return(!hasAnyInconsistency());

    luabridge::LuaRef nameRef         = object["name"];
    luabridge::LuaRef branchRef       = object["branch"];
    luabridge::LuaRef displayNameRef  = object["display_name"];
    luabridge::LuaRef typeRef         = object["type"];
    luabridge::LuaRef costRef         = object["cost"];
    luabridge::LuaRef dependencies    = object["dependencies"];

    if (!dependencies.isNil() && checkForConsistency("dependencies", LUA_TTABLE))
    {
      for (luabridge::Iterator it(dependencies); !it.isNil(); ++it)
      {
        if (it.value().isString())
        {
          m->dependencies.push_back(it.value());
        }
      }
    }

    auto bActivared = false;
    object["activated"] = bActivared;

    m->name         = nameRef.tostring();
    m->branch       = branchRef.tostring();
    m->displayName  = displayNameRef.tostring();
    m->type         = typeRef.tostring();
    m->cost         = costRef;
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
    Multithreading::executeInUiAsync([=](){ m->obActivated->set(activated); });
  }

  osgGaming::Observable<bool>::Ptr LuaSkill::getObActivated() const
  {
    return m->obActivated;
  }

  const std::vector<std::string>& LuaSkill::getDependencies() const
  {
    return m->dependencies;
  }
}
