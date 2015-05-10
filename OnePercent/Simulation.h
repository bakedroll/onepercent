#pragma once

#include "Country.h"
#include "CountriesMap.h"

#include <vector>

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

		unsigned char getCountryId(osg::Vec2f coord);
		std::string getCountryName(osg::Vec2f coord);

	private:
		typedef std::vector<osg::ref_ptr<Country>> CountryList;

		CountryList _countries;
		osg::ref_ptr<CountriesMap> _countriesMap;
	};
}