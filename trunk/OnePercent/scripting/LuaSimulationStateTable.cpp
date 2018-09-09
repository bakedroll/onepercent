#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaCountryState.h"
#include "core/Enums.h"

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

  void LuaSimulationStateTable::triggerObservables()
  {
    traverseElements(static_cast<int>(ModelTraversalType::TRIGGER_OBSERVABLES));
  }

  void LuaSimulationStateTable::onTraverse(int type, luabridge::LuaRef& object)
  {
    if (type != static_cast<int>(ModelTraversalType::INITIALIZE_DATA))
      return;

    assert_return(object.isTable());

    foreachElementDo([this](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isNumber());
      m->countryStates[int(key)] = makeMappedElement<LuaCountryState>(key);
    });
  }
}