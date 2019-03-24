#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaCountry;

  class LuaCountriesTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaCountriesTable>;

    explicit LuaCountriesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaCountriesTable();
    
    std::shared_ptr<LuaCountry> getCountryById(int id) const;

    void addCountry(int id, luabridge::LuaRef& country);
  };
}