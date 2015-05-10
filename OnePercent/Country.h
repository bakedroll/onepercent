#pragma once

#include <osg/Referenced>
#include <osg/Vec3i>

#include <string>

namespace onep
{
	class Country : public osg::Referenced
	{
	public:
		Country(std::string name, unsigned char id, float population, int bip);

		std::string getCountryName();
		unsigned char getId();

	private:
		std::string _name;

		float _populationInMio;
		int _bipInMio;
		unsigned char _id;
	};
}