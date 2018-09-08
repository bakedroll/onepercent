#include "scripting/LuaCountriesTable.h"

#include "scripting/LuaCountry.h"

namespace onep
{
  LuaCountriesTable::LuaCountriesTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
  {
    assert_return(object.isTable());

    makeAllMappedElements<LuaCountry>();
  }

  LuaCountriesTable::~LuaCountriesTable() = default;
}