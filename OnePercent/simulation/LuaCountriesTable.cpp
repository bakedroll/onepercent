#include "simulation/LuaCountriesTable.h"

#include "simulation/Country.h"

namespace onep
{
  LuaCountriesTable::LuaCountriesTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
  {
    assert_return(object.isTable());

    makeAllMappedElements<Country>();
  }

  LuaCountriesTable::~LuaCountriesTable() = default;
}