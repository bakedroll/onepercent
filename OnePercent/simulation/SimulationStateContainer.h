#pragma once

#include <osgGaming/Injector.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class SimulationState;

  class SimulationStateContainer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<SimulationStateContainer> Ptr;

    SimulationStateContainer(osgGaming::Injector& injector);
    ~SimulationStateContainer();

    osg::ref_ptr<SimulationState> getState();

    void loadFromLua(const luabridge::LuaRef object);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}