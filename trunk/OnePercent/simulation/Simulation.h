#pragma once

#include "Skill.h"

#include <osg/Referenced>

#include "nodes/GlobeModel.h"
#include "SkillBranch.h"

namespace onep
{
	class Simulation : public osg::Referenced
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

		//void printStats(bool onlyActivated = false);
	private:
		//Country::Map _countries;
		SkillBranch::Map m_skillBranches;
    GlobeModel::Ptr m_globeModel;

		int m_day;
	};
}