#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaCountry.h"
#include "core/Enums.h"

namespace onep
{
  LuaCountriesTable::LuaCountriesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
  }

  LuaCountriesTable::~LuaCountriesTable() = default;

  void LuaCountriesTable::updateNeighbours()
  {
    traverseElements(static_cast<int>(ModelTraversalType::UPDATE_NEIGHBOURS));
  }

  std::shared_ptr<LuaCountry> LuaCountriesTable::getCountryById(int id) const
  {
    return getMappedElement<LuaCountry>(id);
  }

  void LuaCountriesTable::onTraverse(int type, luabridge::LuaRef& object)
  {
    if (type != static_cast<int>(ModelTraversalType::INITIALIZE_DATA))
      return;

    assert_return(object.isTable());
    makeAllMappedElements<LuaCountry>();
  }
}