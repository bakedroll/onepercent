#pragma once

#include "Country.h"
#include "CountriesMap.h"
#include "AbstractSkill.h"

#include <map>

#include <osg/Referenced>
#include <osg/Vec2f>

namespace onep
{
	class Simulation : public osg::Referenced
	{
	public:
		Simulation();

		void loadCountries();
		void loadSkillsXml(std::string filename);
		const Country::Map& getCountryMap();

		osg::ref_ptr<Country> getCountry(unsigned char id);
		osg::ref_ptr<Country> getCountry(osg::Vec2f coord);
		unsigned char getCountryId(osg::Vec2f coord);
		std::string getCountryName(osg::Vec2f coord);

		osg::ref_ptr<AbstractSkill> getSkill(int id);
		int getNumSkills();

		int getDay();

		void step();

		void printStats(bool onlyActivated = false);
	private:
		Country::Map _countries;
		AbstractSkill::Map _skills;

		osg::ref_ptr<CountriesMap> _countriesMap;

		int _day;
	};
}