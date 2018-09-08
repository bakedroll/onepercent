#include "scripting/LuaSkill.h"
#include "core/Multithreading.h"

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

  LuaSkill::LuaSkill(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef nameRef         = object["name"];
    luabridge::LuaRef displayNameRef  = object["display_name"];
    luabridge::LuaRef typeRef         = object["type"];
    luabridge::LuaRef costRef         = object["cost"];
    luabridge::LuaRef activatedRef    = object["activated"];

    assert_return(nameRef.isString());
    assert_return(displayNameRef.isString());
    assert_return(typeRef.isString());
    assert_return(costRef.isNumber());
    assert_return(activatedRef.type() == LUA_TBOOLEAN);

    m->name         = nameRef.tostring();
    m->displayName  = displayNameRef.tostring();
    m->type         = typeRef.tostring();
    m->cost         = costRef;
    m->obActivated->set(bool(activatedRef));
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

  void LuaSkill::onUpdate(luabridge::LuaRef& object)
  {
    bool activated = bool(object["activated"]);
    if (activated != getIsActivated())
      setIsActivated(activated);
  }
}