#pragma once

#include "Skill.h"

#include "nodes/GlobeModel.h"
#include "SkillBranch.h"
#include "SimulationVisitor.h"
#include "SimulationCallback.h"

namespace onep
{
	class Simulation : public osg::Group, public SimulationCallback
	{
	public:
		Simulation();

    void loadCountries(std::string filename);
		void loadSkillsXml(std::string filename);

		osg::ref_ptr<Skill> getSkill(int id);
		int getNumSkills();

		int getDay();

    void setGlobeModel(GlobeModel::Ptr model);

		void step();

    virtual bool callback(SimulationVisitor* visitor) override;

		//void printStats(bool onlyActivated = false);
	private:
		SkillBranch::Map m_skillBranches;
    Skill::Map m_skills;

    GlobeModel::Ptr m_globeModel;

    SimulationVisitor::Ptr m_applySkillsVisitor;
    SimulationVisitor::Ptr m_affectNeighborsVisitor;
    SimulationVisitor::Ptr m_progressCountriesVisitor;

		int m_day;
	};
}