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

    void loadCountries(std::string filename);
		void loadSkillsXml(std::string filename);

		int getNumSkills();
    SkillBranch::Ptr getSkillBranch(BranchType type);

		int getDay();

    void setGlobeModel(GlobeModel::Ptr model);

		void step();

    virtual bool callback(SimulationVisitor* visitor) override;

	private:
		SkillBranch::Map m_skillBranches;

		GlobeModel::Ptr m_globeModel;

		SimulationVisitor::Ptr m_applySkillsVisitor;
		SimulationVisitor::Ptr m_affectNeighborsVisitor;
		SimulationVisitor::Ptr m_progressCountriesVisitor;

		int m_day;
	};
}