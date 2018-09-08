#include "SimulationState.h"

#include "simulation/CountryState.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct SimulationState::Impl
  {
    Impl() {}

    CountryState::Map countryStates;
  };

  SimulationState::SimulationState(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    foreachElementDo([this](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(key.isNumber());

      m->countryStates[int(key)] = makeMappedElement<CountryState>(key);
    });
  }

  SimulationState::~SimulationState() = default;

  CountryState::Map& SimulationState::getCountryStates() const
  {
    return m->countryStates;
  }

  CountryState::Ptr SimulationState::getCountryState(int cid) const
  {
    return getMappedElement<CountryState>(cid);
  }
}