#include "SimulatedValuesContainer.h"

#include "core/Macros.h"

#include <QString>

namespace onep
{
  struct SimulatedValuesContainer::Impl
  {
    Impl(osgGaming::Injector& injector)
      : skillsContainer(injector.inject<SkillsContainer>())
      , state(new SimulationState())
    {}

    SkillsContainer::Ptr skillsContainer;
    SimulationState::Ptr state;
  };

  SimulatedValuesContainer::SimulatedValuesContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
  }

  SimulatedValuesContainer::~SimulatedValuesContainer()
  {
  }

  SimulationState::Ptr SimulatedValuesContainer::getState() const
  {
    return m->state;
  }

  void SimulatedValuesContainer::setState(SimulationState::Ptr state)
  {
    m->state = state;
  }

  void SimulatedValuesContainer::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<SimulatedValuesContainer>("SimulatedValuesContainer")
      .addFunction("add_values", &SimulatedValuesContainer::lua_add_values)
      .endClass();
  }

  std::string SimulatedValuesContainer::instanceVariableName()
  {
    return "valuesContainer";
  }

  void SimulatedValuesContainer::lua_add_values(lua_State* state)
  {
    enum ValueType
    { DEFAULT, BRANCH, UNKNOWN } etype;

    luaL_checktype(state, -1, LUA_TTABLE);

    lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
      luaL_checktype(state, -1, LUA_TTABLE);
      lua_getfield(state, -1, "name");
      lua_getfield(state, -2, "type");
      lua_getfield(state, -3, "init");

      const char* name = luaL_checkstring(state, -3);
      const char* type = luaL_checkstring(state, -2);
      int init = luaL_checkinteger(state, -1);
      
      if (strcmp(type, "default") == 0)
        etype = DEFAULT;
      else if (strcmp(type, "branch") == 0)
        etype = BRANCH;
      else
      {
        etype = UNKNOWN;
        OSGG_QLOG_WARN(QString("Unknown value type: %1").arg(type));
      }

      if (etype != UNKNOWN)
      {
        ONEP_FOREACH(CountryState::Map, it, m->state->getCountryStates())
        {
          if (etype == DEFAULT)
            it->second->addValue(name, init);
          else if (etype == BRANCH)
            it->second->addBranchValue(name, m->skillsContainer, init);
        }

        OSGG_QLOG_INFO(QString("Value added: %1").arg(name));
      }

      lua_pop(state, 4);
    }
  }
}