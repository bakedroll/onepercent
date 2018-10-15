#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaCountry.h"
#include "core/Enums.h"

namespace onep
{
  LuaCountriesTable::LuaCountriesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
    assert_return(luaref().isTable());
  }

  LuaCountriesTable::~LuaCountriesTable() = default;

  std::shared_ptr<LuaCountry> LuaCountriesTable::getCountryById(int id) const
  {
    return getMappedElement<LuaCountry>(id);
  }

  void LuaCountriesTable::addCountry(int id, luabridge::LuaRef& country)
  {
    addMappedElement<LuaCountry>(id, country);
  }
}