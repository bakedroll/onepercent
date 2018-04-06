#pragma once

#include "scripting/LuaStateManager.h"
#include "simulation/SimulationState.h"

#include <osg/Referenced>

#include <osgGaming/Injector.h>

namespace onep
{
  class SimulatedValuesContainer : public osg::Referenced, public LuaClassInstance
  {
  public:
    typedef osg::ref_ptr<SimulatedValuesContainer> Ptr;

    explicit SimulatedValuesContainer(osgGaming::Injector& injector);
    ~SimulatedValuesContainer();

    SimulationState::Ptr getState() const;
    void setState(SimulationState::Ptr state);

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    void lua_add_values(lua_State* state);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}