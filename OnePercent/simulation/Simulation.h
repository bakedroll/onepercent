#pragma once

#include "core/LuaStateManager.h"

namespace onep
{
	class Simulation : public osg::Referenced, public LuaClassInstance
	{
	public:
    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgGaming::Injector& injector);
    ~Simulation();

    void prepare();

    const std::map<int, std::string>& getIdCountryMap();

    bool paySkillPoints(int points);

    void start();
    void stop();
    bool running() const;

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    // Lua functions
    void lua_start(lua_State* state);
    void lua_stop(lua_State* state);

    void lua_add_countries(lua_State* state);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}