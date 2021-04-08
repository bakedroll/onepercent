#include "scripting/LuaCountry.h"

#include <osgHelper/Macros.h>

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
    m->id   = getNumber<int>("id");
    m->name = getString("name");

    if (object["init_values"].isNil())
    {
      object["init_values"] = luabridge::newTable(luaState);
    }
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
    luabridge::LuaRef initValuesRef = luaRef()["init_values"];
    luabridge::LuaRef initValueRef  = initValuesRef[valueName];

    if (initValueRef.isNil())
    {
      return defaultValue;
    }

    assert_return(initValueRef.isNumber(), defaultValue);
    return initValueRef;
  }
}