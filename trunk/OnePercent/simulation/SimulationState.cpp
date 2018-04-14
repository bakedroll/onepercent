#include "SimulationState.h"

#include "core/Macros.h"
#include "simulation/CountryState.h"

namespace onep
{
  struct SimulationState::Impl
  {
    Impl() {}

    CountryState::Map countryStates;
  };

  SimulationState::SimulationState(const luabridge::LuaRef& object)
    : osg::Referenced()
    , LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    for (luabridge::Iterator it(object); !it.isNil(); ++it)
    {
      luabridge::LuaRef ref = *it;
      assert_continue(ref.isTable());

      m->countryStates[int(it.key())] = new CountryState(ref);
    }
  }

  SimulationState::~SimulationState()
  {
  }

  void SimulationState::addCountryState(int id, CountryState::Ptr countryState)
  {
    m->countryStates[id] = countryState;
  }

  CountryState::Map& SimulationState::getCountryStates()
  {
    return m->countryStates;
  }

  void SimulationState::writeObject(luabridge::LuaRef& object) const
  {
    for (CountryState::Map::const_iterator it = m->countryStates.cbegin(); it != m->countryStates.cend(); ++it)
      it->second->write();
  }

  void SimulationState::readObject(const luabridge::LuaRef& object)
  {
    for (CountryState::Map::const_iterator it = m->countryStates.cbegin(); it != m->countryStates.cend(); ++it)
      it->second->read();
  }
}