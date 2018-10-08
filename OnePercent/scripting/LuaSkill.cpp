#include "scripting/LuaSkill.h"
#include "core/Multithreading.h"
#include "core/Enums.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaSkill::Impl
  {
    Impl()
      : obActivated(new osgGaming::Observable<bool>(false))
    {}

    std::string name;
    std::string displayName;
    std::string type;
    int cost;

    osgGaming::Observable<bool>::Ptr obActivated;
  };

  LuaSkill::LuaSkill(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef nameRef         = object["name"];
    luabridge::LuaRef displayNameRef  = object["display_name"];
    luabridge::LuaRef typeRef         = object["type"];
    luabridge::LuaRef costRef         = object["cost"];

    assert_return(nameRef.isString());
    assert_return(displayNameRef.isString());
    assert_return(typeRef.isString());
    assert_return(costRef.isNumber());

    auto bActivared = false;
    object["activated"] = bActivared;

    m->name         = nameRef.tostring();
    m->displayName  = displayNameRef.tostring();
    m->type         = typeRef.tostring();
    m->cost         = costRef;
    m->obActivated->set(bActivared);

    addVisitorFunc(static_cast<int>(ModelTraversalType::TRIGGER_OBSERVABLES), [this](luabridge::LuaRef&)
    {
      bool activated = bool(luaref()["activated"]);
      if (activated != getIsActivated())
        setIsActivated(activated);
    });
  }

  LuaSkill::~LuaSkill() = default;

  std::string LuaSkill::getSkillName() const
  {
    return m->name;
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
    luaref()["activated"] = activated;
    Multithreading::uiExecuteOrAsync([=](){ m->obActivated->set(activated); });
  }

  osgGaming::Observable<bool>::Ptr LuaSkill::getObActivated() const
  {
    return m->obActivated;
  }
}