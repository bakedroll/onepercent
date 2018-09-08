#pragma once

#include "scripting/LuaStateManager.h"
#include "simulation/CountryState.h"

namespace onep
{
  class SimulationState : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<SimulationState> Ptr;

    explicit SimulationState(const luabridge::LuaRef& object);
    ~SimulationState();

    CountryState::Map& getCountryStates() const;
    CountryState::Ptr getCountryState(int cid) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}