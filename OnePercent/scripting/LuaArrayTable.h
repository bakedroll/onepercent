#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{

  template <typename LuaObject,
      typename = typename std::enable_if<std::is_base_of<LuaObjectMapper, LuaObject>::value>::type>
  class LuaArrayTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaArrayTable<LuaObject>>;

    LuaArrayTable(const luabridge::LuaRef& object, lua_State* luaState)
        : LuaObjectMapper(object, luaState)
    {
      
    }

    ~LuaArrayTable() = default;

    void addEement(luabridge::LuaRef& ref)
    {
      appendMappedElement<LuaObject>(ref);
    }

  };
}