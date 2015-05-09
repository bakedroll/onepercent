#pragma once

#include <osg/Referenced>
#include <osg/Vec3i>

#include <string>

namespace onep
{
	class Country : public osg::Referenced
	{
	public:
		Country(std::string name, osg::Vec3i color, float population, int bip);

		std::string getCountryName();
		osg::Vec3i getColor();

	private:
		std::string _name;

		float _populationInMio;
		int _bipInMio;
		osg::Vec3i _color;
	};
}