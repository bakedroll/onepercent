#pragma once

#include "scripting/LuaTableMappedObject.h"

namespace onep
{

  template <typename LuaObject,
      typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
  class LuaArrayTable : public LuaTableMappedObject
  {
  public:
    using Ptr = std::shared_ptr<LuaArrayTable<LuaObject>>;

    LuaArrayTable(const luabridge::LuaRef& object, lua_State* luaState)
        : LuaTableMappedObject(object, luaState)
    {
      
    }

    ~LuaArrayTable() = default;

    void addEement(luabridge::LuaRef& ref)
    {
      appendMappedElement<LuaObject>(ref);
    }

  };
}