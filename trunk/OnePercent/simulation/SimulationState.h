#pragma once

#include "scripting/LuaStateManager.h"
#include "simulation/CountryState.h"

namespace onep
{
  class SimulationState : public osg::Referenced, public LuaObjectMapper
  {
  public:
    typedef osg::ref_ptr<SimulationState> Ptr;

    SimulationState(const luabridge::LuaRef& object);
    ~SimulationState();

    CountryState::Map& getCountryStates();

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override;
    virtual void readObject(const luabridge::LuaRef& object) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}