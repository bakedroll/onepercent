#pragma once

#include "scripting/LuaStateManager.h"
#include "scripting/LuaCountryState.h"

namespace onep
{
  class LuaSimulationState : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaSimulationState> Ptr;

    explicit LuaSimulationState(const luabridge::LuaRef& object);
    ~LuaSimulationState();

    LuaCountryState::Map& getCountryStates() const;
    LuaCountryState::Ptr getCountryState(int cid) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}