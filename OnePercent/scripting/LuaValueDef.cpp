#include "scripting/LuaValueDef.h"

#include <QString>

namespace onep
{
  struct LuaValueDef::Impl
  {
    Impl()
      : init(0.0)
      , isVisible(false)
    {}

    std::string name;
    std::string group;
    float       init;
    bool        isVisible;
  };

  LuaValueDef::LuaValueDef(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    checkForConsistency("name", LUA_TSTRING);
    checkForConsistency("init", LUA_TNUMBER);

    assert_return(!hasAnyInconsistency());

    luabridge::LuaRef nameRef = object["name"];
    luabridge::LuaRef initRef = object["init"];
    luabridge::LuaRef visible = object["visible"];
    luabridge::LuaRef group   = object["group"];

    m->name = nameRef.tostring();
    m->init = initRef;

    if (!visible.isNil() && checkForConsistency("visible", LUA_TBOOLEAN))
    {
        m->isVisible = static_cast<bool>(visible);
    }

    if (!group.isNil() && checkForConsistency("group", LUA_TSTRING))
    {
        m->group = group.tostring();
    }
  }

  LuaValueDef::~LuaValueDef() = default;

  std::string LuaValueDef::getName() const
  {
    return m->name;
  }

  std::string LuaValueDef::getGroup() const
  {
    return m->group;
  }

  float LuaValueDef::getInit() const
  {
    return m->init;
  }

  bool LuaValueDef::getIsVisible() const
  {
    return m->isVisible;
  }
}
