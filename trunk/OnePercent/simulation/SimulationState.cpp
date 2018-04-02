#include "SimulationState.h"

#include "simulation/CountryState.h"

namespace onep
{
  struct SimulationState::Impl
  {
    Impl() {}

    CountryState::Map countryStates;
  };

  SimulationState::SimulationState()
    : osg::Referenced()
    , LuaClass()
    , m(new Impl())
  {

  }

  SimulationState::~SimulationState()
  {
  }

  SimulationState::Ptr SimulationState::copy() const
  {
    Ptr c = new SimulationState();
    
    for (CountryState::Map::iterator it = m->countryStates.begin(); it != m->countryStates.end(); ++it)
      c->m->countryStates[it->first] = it->second->copy();

    return c;
  }

  void SimulationState::overwrite(Ptr other)
  {
    for (CountryState::Map::iterator it = other->m->countryStates.begin(); it != other->m->countryStates.end(); ++it)
      m->countryStates[it->first]->overwrite(it->second);
  }

  void SimulationState::addCountryState(int id, CountryState::Ptr countryState)
  {
    m->countryStates[id] = countryState;
  }

  CountryState::Map& SimulationState::getCountryStates()
  {
    return m->countryStates;
  }

  void SimulationState::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<SimulationState>("SimulationState")
      .addFunction("get_country_state", &SimulationState::lua_get_country_state)
      .endClass();
  }

  CountryState* SimulationState::lua_get_country_state(int id)
  {
    return m->countryStates[id].get();
  }
}