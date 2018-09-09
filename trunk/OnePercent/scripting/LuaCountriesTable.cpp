#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaCountry.h"
#include "core/Enums.h"

namespace onep
{
  LuaCountriesTable::LuaCountriesTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
  {
    addVisitorFunc(static_cast<int>(ModelTraversalType::INITIALIZE_DATA), [this](luabridge::LuaRef&)
    {
      assert_return(luaref().isTable());
      makeAllMappedElements<LuaCountry>();
    });
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
}