#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaCountryState.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaSimulationStateTable::Impl
  {
    Impl() {}

    LuaCountryState::Map countryStates;
  };

  LuaSimulationStateTable::LuaSimulationStateTable(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    foreachElementDo([this](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isNumber());

      m->countryStates[int(key)] = makeMappedElement<LuaCountryState>(key);
    });
  }

  LuaSimulationStateTable::~LuaSimulationStateTable() = default;

  LuaCountryState::Map& LuaSimulationStateTable::getCountryStates() const
  {
    return m->countryStates;
  }

  LuaCountryState::Ptr LuaSimulationStateTable::getCountryState(int cid) const
  {
    return getMappedElement<LuaCountryState>(cid);
  }
}