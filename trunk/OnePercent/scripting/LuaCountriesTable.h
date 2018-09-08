#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaCountriesTable : public LuaObjectMapper
  {
  public:
    explicit LuaCountriesTable(const luabridge::LuaRef& object);
    ~LuaCountriesTable();

  };
}