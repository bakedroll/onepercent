#pragma once

#include "scripting/LuaStateManager.h"

#include <osgGaming/Observable.h>

namespace onep
{
  class SimulationState;
  class UpdateThread;

	class Simulation : public osg::Referenced, public LuaClassInstance
	{
	public:
    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgGaming::Injector& injector);
    ~Simulation();

    void prepare();
    void shutdownUpdateThread();

    UpdateThread* getUpdateThread();

    bool paySkillPoints(int points);

    void start();
    void stop();
    bool running() const;
    osgGaming::Observable<bool>::Ptr getORunning() const;

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    // Lua functions
    void lua_start(lua_State* state);
    void lua_stop(lua_State* state);
    void lua_set_skill_points(int points);
    void lua_set_day(int day);
    void lua_set_interval(int interval);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}