#pragma once

#include <osgGaming/Injector.h>

#include <functional>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class LuaSimulationState;

  class SimulationStateContainer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<SimulationStateContainer> Ptr;

    SimulationStateContainer(osgGaming::Injector& injector);
    ~SimulationStateContainer();

    void accessState(std::function<void(std::shared_ptr<LuaSimulationState>)> func);

    void loadFromLua(const luabridge::LuaRef object);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}