#pragma once

#include "core/Observables.h"

#include <osg/Referenced>
#include <osgHelper/ioc/Injector.h>

#include <luaHelper/LuaBridgeDefinition.h>

extern "C"
{
#include <lua.h>
}

namespace onep
{
  class Simulation;

  class LuaSimulation : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<LuaSimulation>;

    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    LuaSimulation(osgHelper::ioc::Injector& injector);
    ~LuaSimulation();

    void lua_start(lua_State* state);
    void lua_stop(lua_State* state);
    void lua_set_skill_points(int points);
    void lua_add_skill_points(int points);
    void lua_set_day(int day);
    void lua_set_interval(int interval);
    void lua_set_tick_update_mode(int mode);
    void lua_set_profiling_logs_enabled(bool enabled);

  private:
    osg::ref_ptr<Simulation> m_simulation;
    ODay::Ptr m_oDay;
    ONumSkillPoints::Ptr m_oNumSkillPoints;

  };
}