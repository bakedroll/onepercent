#pragma once

#include "Country.h"
#include "CountriesMap.h"

#include <map>

#include <osg/Object>
#include <osg/Referenced>
#include <osg/Vec2f>

namespace onep
{
	class Simulation : public osg::Referenced
	{
	public:
		Simulation();

		void loadCountries();

		osg::ref_ptr<Country> getCountry(unsigned char id);
		unsigned char getCountryId(osg::Vec2f coord);
		std::string getCountryName(osg::Vec2f coord);

		int getDay();

		void step();

		void printStats();

	private:
		typedef std::map<unsigned char, osg::ref_ptr<Country>> CountryMap;

		CountryMap _countries;
		osg::ref_ptr<CountriesMap> _countriesMap;

		int _day;
	};
}