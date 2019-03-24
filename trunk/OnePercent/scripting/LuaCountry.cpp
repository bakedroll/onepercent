#include "scripting/LuaCountry.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaCountry::Impl
  {
    Impl() : id(-1) {}
    ~Impl() = default;

    int         id;
    std::string name;
  };

  LuaCountry::LuaCountry(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    checkForConsistency("id", LUA_TNUMBER);
    checkForConsistency("name", LUA_TSTRING);

    assert_return(!hasAnyInconsistency());

    luabridge::LuaRef idRef   = object["id"];
    luabridge::LuaRef nameRef = object["name"];

    if (object["init_values"].isNil())
    {
      object["init_values"] = luabridge::newTable(luaState);
    }

    m->id   = idRef;
    m->name = nameRef.tostring();
  }

  LuaCountry::~LuaCountry() = default;

  int LuaCountry::getId() const
  {
    return m->id;
  }

  const std::string& LuaCountry::getName() const
  {
    return m->name;
  }

  float LuaCountry::getInitValue(const std::string& valueName, float defaultValue) const
  {
    luabridge::LuaRef initValuesRef = luaref()["init_values"];
    luabridge::LuaRef initValueRef  = initValuesRef[valueName];

    if (initValueRef.isNil())
    {
      return defaultValue;
    }

    assert_return(initValueRef.isNumber(), defaultValue);
    return initValueRef;
  }
}