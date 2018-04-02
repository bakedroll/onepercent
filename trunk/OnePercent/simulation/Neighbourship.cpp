#include "Neighbourship.h"

#include "simulation/CountryState.h"

namespace onep
{
  struct Neighbourship::Impl
  {
    Impl() {}

    std::vector<CountryState*> neighbourStates;
  };

  Neighbourship::Neighbourship()
    : osg::Referenced()
    , LuaClass()
    , m(new Impl())
  {
  }

  Neighbourship::~Neighbourship()
  {
  }

  void Neighbourship::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<Neighbourship>("Neighbourship")
      .addFunction("get_num_neighbours", &Neighbourship::lua_get_num_neighbours)
      .addFunction("get_neighbour_state", &Neighbourship::lua_get_neighbour_state)
      .addFunction("set_current_branch", &Neighbourship::lua_set_current_branch)
      .endClass();
  }

  void Neighbourship::addNeighbour(CountryState* state)
  {
    m->neighbourStates.push_back(state);
  }

  int Neighbourship::lua_get_num_neighbours() const
  {
    return int(m->neighbourStates.size());
  }

  CountryState* Neighbourship::lua_get_neighbour_state(int i)
  {
    return m->neighbourStates[i];
  }

  void Neighbourship::lua_set_current_branch(std::string branchName)
  {
    for (std::vector<CountryState*>::iterator it = m->neighbourStates.begin(); it != m->neighbourStates.end(); ++it)
      (*it)->lua_set_current_branch(branchName);
  }
}