#include "scripting/LuaSkill.h"
#include "core/Multithreading.h"

#include <osgGaming/Macros.h>

namespace onep
{
  void LuaSkill::Definition::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
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
      : obActivated(new osgGaming::Observable<bool>(false))
    {}

    std::string name;
    std::string branch;
    std::string displayName;
    std::string type;
    int cost;

    osgGaming::Observable<bool>::Ptr obActivated;
  };

  LuaSkill::LuaSkill(const luabridge::LuaRef& object)
    : LuaCallbackRegistry()
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef nameRef         = object["name"];
    luabridge::LuaRef branchRef       = object["branch"];
    luabridge::LuaRef displayNameRef  = object["display_name"];
    luabridge::LuaRef typeRef         = object["type"];
    luabridge::LuaRef costRef         = object["cost"];

    assert_return(nameRef.isString());
    assert_return(branchRef.isString());
    assert_return(displayNameRef.isString());
    assert_return(typeRef.isString());
    assert_return(costRef.isNumber());

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

  std::string LuaSkill::getName() const
  {
    return m->name;
  }

  std::string LuaSkill::getBranchName() const
  {
    return m->branch;
  }

  std::string LuaSkill::getDisplayName() const
  {
    return m->displayName;
  }

  bool LuaSkill::getIsActivated() const
  {
    return m->obActivated->get();
  }

  void LuaSkill::setIsActivated(bool activated)
  {
    Multithreading::uiExecuteOrAsync([=](){ m->obActivated->set(activated); });
  }

  osgGaming::Observable<bool>::Ptr LuaSkill::getObActivated() const
  {
    return m->obActivated;
  }
}