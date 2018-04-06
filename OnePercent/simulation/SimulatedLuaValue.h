#pragma once

#include "simulation/SimulatedValue.h"
#include "scripting/LuaStateManager.h"

namespace onep
{
  class SimulatedLuaValue : public SimulatedValue<float>, public LuaClass
  {
  public:
    typedef osg::ref_ptr<SimulatedLuaValue> Ptr;

    SimulatedLuaValue();
    explicit SimulatedLuaValue(const float& initial);
    ~SimulatedLuaValue();

    virtual void registerClass(lua_State* state) override;
    
    const float& lua_get() const;
    void lua_set(const float& value);

  };
}