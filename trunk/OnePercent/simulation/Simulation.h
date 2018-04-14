#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class SimulationState;

	class Simulation : public osg::Referenced, public LuaClassInstance
	{
	public:
    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgGaming::Injector& injector);
    ~Simulation();

    void prepare();
    void shutdownUpdateThread();

    bool paySkillPoints(int points);

    void start();
    void stop();
    bool running() const;

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    // Lua functions
    void lua_start(lua_State* state);
    void lua_stop(lua_State* state);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}