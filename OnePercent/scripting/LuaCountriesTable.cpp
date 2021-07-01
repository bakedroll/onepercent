#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaCountry.h"

namespace onep
{
  LuaCountriesTable::LuaCountriesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
  {
  }

  LuaCountriesTable::~LuaCountriesTable() = default;

  std::shared_ptr<LuaCountry> LuaCountriesTable::getCountryById(int id) const
  {
    return getMappedObject<LuaCountry>(id);
  }

  void LuaCountriesTable::addCountry(int id, luabridge::LuaRef& country)
  {
    addMappedObject<LuaCountry>(id, country);
  }
}