#pragma once

#include "scripting/LuaStateManager.h"
#include "simulation/SimulatedLuaValue.h"
#include "simulation/CountryState.h"

namespace onep
{
  class SimulationState : public osg::Referenced, public LuaClass
  {
  public:
    typedef osg::ref_ptr<SimulationState> Ptr;

    SimulationState();
    ~SimulationState();

    Ptr copy() const;
    void overwrite(Ptr other);

    void addCountryState(int id, CountryState::Ptr countryState);
    CountryState::Map& getCountryStates();

    virtual void registerClass(lua_State* state) override;

    CountryState* lua_get_country_state(int id);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}