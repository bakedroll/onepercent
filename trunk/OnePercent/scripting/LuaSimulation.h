#pragma once

#include "core/Observables.h"
#include "scripting/LuaClassInstance.h"

#include <osg/Referenced>
#include <osgGaming/Injector.h>

namespace onep
{
  class Simulation;

  class LuaSimulation : public osg::Referenced, public LuaClassInstance
  {
  public:
    LuaSimulation(osgGaming::Injector& injector);
    ~LuaSimulation();

    void lua_start(lua_State* state);
    void lua_stop(lua_State* state);
    void lua_set_skill_points(int points);
    void lua_add_skill_points(int points);
    void lua_set_day(int day);
    void lua_set_interval(int interval);

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

  private:
    osg::ref_ptr<Simulation> m_simulation;
    ODay::Ptr m_oDay;
    ONumSkillPoints::Ptr m_oNumSkillPoints;

  };
}