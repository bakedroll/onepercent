#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaArrayTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaArrayTable>;

    LuaArrayTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaArrayTable();

    void addEement(const luabridge::LuaRef& ref);

  };
}