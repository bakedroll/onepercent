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

  LuaSimulationStateTable::LuaSimulationStateTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());
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

  void LuaSimulationStateTable::addCountryState(int id)
  {
    m->countryStates[id] = newMappedElement<LuaCountryState>(id);
  }

  void LuaSimulationStateTable::updateObservables()
  {
    for (const auto& state : m->countryStates)
    {
      state.second->getBranchesActivatedTable()->updateObservables();
    }
  }
}