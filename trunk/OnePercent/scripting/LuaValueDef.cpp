#include "scripting/LuaValueDef.h"

namespace onep
{
  struct LuaValueDef::Impl
  {
    Impl() {}

    std::string name;
    std::string type;
    float init;
  };

  LuaValueDef::LuaValueDef(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef nameRef = object["name"];
    luabridge::LuaRef typeRef = object["type"];
    luabridge::LuaRef initRef = object["init"];

    assert_return(nameRef.isString());
    assert_return(typeRef.isString());
    assert_return(initRef.isNumber());

    m->name = nameRef.tostring();
    m->type = typeRef.tostring();
    m->init = initRef;
  }

  LuaValueDef::~LuaValueDef() = default;
}