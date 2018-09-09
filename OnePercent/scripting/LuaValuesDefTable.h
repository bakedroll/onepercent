#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaValuesDefTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaValuesDefTable>;

    explicit LuaValuesDefTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaValuesDefTable();

  };
}