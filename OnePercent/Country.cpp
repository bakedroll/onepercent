#include "Country.h"

using namespace onep;
using namespace osg;
using namespace std;

Country::Country(string name, Vec3i color, float population, int bip)
	: Referenced(),
	  _name(name),
	  _color(color),
	  _populationInMio(population),
	  _bipInMio(bip)
{
	
}

string Country::getCountryName()
{
	return _name;
}

Vec3i Country::getColor()
{
	return _color;
}
