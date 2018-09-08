#include "LuaSimulationState.h"

#include "scripting/LuaCountryState.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaSimulationState::Impl
  {
    Impl() {}

    LuaCountryState::Map countryStates;
  };

  LuaSimulationState::LuaSimulationState(const luabridge::LuaRef& object)
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

  LuaSimulationState::~LuaSimulationState() = default;

  LuaCountryState::Map& LuaSimulationState::getCountryStates() const
  {
    return m->countryStates;
  }

  LuaCountryState::Ptr LuaSimulationState::getCountryState(int cid) const
  {
    return getMappedElement<LuaCountryState>(cid);
  }
}