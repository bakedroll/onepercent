#include "Simulation.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/ByteStream.h>
#include <osg/Image>

using namespace osgGaming;
using namespace onep;
using namespace osg;
using namespace std;

Simulation::Simulation()
	: Referenced(),
	  _day(0)
{
}

void Simulation::loadCountries()
{
	string countriesBinFilename = "./GameData/data/countries.dat";

	char* bytes = ResourceManager::getInstance()->loadBinary(countriesBinFilename);

	ByteStream stream(bytes);

	int ncountries = stream.read<int>();

	for (int i = 0; i < ncountries; i++)
	{
		int name_length = stream.read<int>();
		char* name_p = stream.readString(name_length);
		string name = string(name_p);
		
		float population = stream.read<float>();
		int bip = stream.read<int>();
		unsigned char id = stream.read<unsigned char>();

		ref_ptr<Country> country = new Country(name, id, population, bip);
		_countries.insert(CountryMap::value_type(id, country));

		delete[] name_p;
	}

	int mapWidth = stream.read<int>();
	int mapHeight = stream.read<int>();

	_countriesMap = new CountriesMap(mapWidth, mapHeight, (unsigned char*)&bytes[stream.getPos()]);

	ResourceManager::getInstance()->clearCacheResource(countriesBinFilename);
}

ref_ptr<Country> Simulation::getCountry(unsigned char id)
{
	return _countries.find(id)->second;
}

unsigned char Simulation::getCountryId(Vec2f coord)
{
	Vec2i mapSize = _countriesMap->getSize();

	int ix = (int)(coord.x() * (float)mapSize.x());
	int iy = (int)(coord.y() * (float)mapSize.y());

	return _countriesMap->getDataAt(ix, iy);
}

string Simulation::getCountryName(Vec2f coord)
{
	CountryMap::iterator it = _countries.find(getCountryId(coord));
	if (it == _countries.end())
	{
		return "No country selected";
	}

	return it->second->getCountryName();
}

int Simulation::getDay()
{
	return _day;
}

void Simulation::step()
{
	_day++;
}

void Simulation::printStats()
{
	printf("\n=========================================\n\n");

	for (CountryMap::iterator it = _countries.begin(); it != _countries.end(); ++it)
	{
		string name = it->second->getCountryName();
		int s = 22 - (int)name.size();

		for (int i = 0; i < s; i++)
		{
			name += " ";
		}

		printf("%sBip: %d Mio\n", name.data(), it->second->getBip());
	}

	printf("\n=========================================\n");
}