#pragma once

#include "core/LuaStateManager.h"
#include "nodes/GlobeModel.h"
#include "nodes/CountryMesh.h"

#include "SkillBranch.h"
#include "SimulationVisitor.h"
#include "SimulationCallback.h"

namespace onep
{
	class Simulation : public osg::Group, public SimulationCallback, public LuaClassInstance
	{
	public:
    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgGaming::Injector& injector);
    ~Simulation();

		void loadSkillsXml(std::string filename);

    void attachCountries(CountryMesh::Map& countries);

    SkillBranch::Ptr getSkillBranch(BranchType type);

    bool paySkillPoints(int points);

    void start();
    void stop();
    bool running() const;

    virtual bool callback(SimulationVisitor* visitor) override;
    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    // Lua functions
    void lua_add_branches(lua_State* state);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}