#pragma once

#include "nodes/GlobeModel.h"
#include "SkillBranch.h"
#include "SimulationVisitor.h"
#include "SimulationCallback.h"

namespace onep
{
	class Simulation : public osg::Group, public SimulationCallback
	{
	public:
    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation();
    ~Simulation();

		void loadSkillsXml(std::string filename);

    void attachCountries(CountryMesh::Map& countries);

		int getNumSkills();
    SkillBranch::Ptr getSkillBranch(BranchType type);

		osgGaming::Observable<int>::Ptr getDayObs();

    void start();
    void stop();
    bool running() const;

    virtual bool callback(SimulationVisitor* visitor) override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}