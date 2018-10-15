#include "scripting/LuaCountry.h"
#include "core/Enums.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaCountry::Impl
  {
    Impl() {}
    ~Impl() {}

    int id;
    std::string name;
  };

  LuaCountry::LuaCountry(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef idRef = object["id"];
    luabridge::LuaRef nameRef = object["name"];

    assert_return(idRef.isNumber());
    assert_return(nameRef.isString());

    if (object["init_values"].isNil())
    {
      object["init_values"] = luabridge::newTable(luaState);
    }

    m->id = idRef;
    m->name = nameRef.tostring();
  }

  LuaCountry::~LuaCountry() = default;

  int LuaCountry::getId() const
  {
    return m->id;
  }

  std::string LuaCountry::getName() const
  {
    return m->name;
  }

  float LuaCountry::getInitValue(const std::string valueName, float defaultValue) const
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