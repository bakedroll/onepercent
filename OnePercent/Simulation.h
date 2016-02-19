#pragma once

#include "Country.h"
#include "CountriesMap.h"
#include "AbstractSkill.h"

#include <map>

#include <osg/Referenced>
#include <osg/Vec2f>
#include "GlobeModel.h"

namespace onep
{
	class Simulation : public osg::Referenced
	{
	public:
		Simulation();

    void loadCountries(std::string filename);
		void loadSkillsXml(std::string filename);

		osg::ref_ptr<AbstractSkill> getSkill(int id);
		int getNumSkills();

		int getDay();

    void setGlobeModel(GlobeModel::Ptr model);

		void step();

		void printStats(bool onlyActivated = false);
	private:
		//Country::Map _countries;
		AbstractSkill::Map m_skills;
    GlobeModel::Ptr m_globeModel;

		//osg::ref_ptr<CountriesMap> _countriesMap;

		int m_day;
	};
}