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
    : osg::Referenced()
    , LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    for (luabridge::Iterator it(object); !it.isNil(); ++it)
    {
      luabridge::LuaRef ref = *it;
      assert_continue(ref.isTable());
      assert_continue(it.key().isNumber());

      m->countryStates[int(it.key())] = new CountryState(ref);
    }
  }

  SimulationState::~SimulationState()
  {
  }

  CountryState::Map& SimulationState::getCountryStates() const
  {
    return m->countryStates;
  }

  CountryState::Ptr SimulationState::getCountryState(int cid) const
  {
    assert_return(m->countryStates.count(cid) > 0, nullptr);
    return m->countryStates[cid];
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