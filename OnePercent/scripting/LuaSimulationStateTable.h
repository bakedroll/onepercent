#pragma once

#include "scripting/LuaCountryState.h"

namespace onep
{
  class LuaSimulationStateTable : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaSimulationStateTable> Ptr;

    explicit LuaSimulationStateTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSimulationStateTable();

    LuaCountryState::Map& getCountryStates() const;
    LuaCountryState::Ptr getCountryState(int cid) const;

    void triggerObservables();

  protected:
    virtual void onTraverse(int type, luabridge::LuaRef& object) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}