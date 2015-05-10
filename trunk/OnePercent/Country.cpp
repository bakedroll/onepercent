#include "Country.h"

using namespace onep;
using namespace osg;
using namespace std;

Country::Country(string name, unsigned char id, float population, int bip)
	: Referenced(),
	  _name(name),
	  _id(id),
	  _populationInMio(population),
	  _bipInMio(bip)
{
	
}

string Country::getCountryName()
{
	return _name;
}

unsigned char Country::getId()
{
	return _id;
}
