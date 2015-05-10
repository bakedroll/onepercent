#include "Simulation.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/ByteStream.h>
#include <osg/Image>

using namespace osgGaming;
using namespace onep;
using namespace osg;
using namespace std;

Simulation::Simulation()
	: Referenced()
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
		_countries.push_back(country);

		delete[] name_p;
	}

	int mapWidth = stream.read<int>();
	int mapHeight = stream.read<int>();

	_countriesMap = new CountriesMap(mapWidth, mapHeight, (unsigned char*)&bytes[stream.getPos()]);

	ResourceManager::getInstance()->clearCacheResource(countriesBinFilename);
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
	unsigned char id = getCountryId(coord);

	for (CountryList::iterator it = _countries.begin(); it != _countries.end(); ++it)
	{
		if (id == it->get()->getId())
		{
			return it->get()->getCountryName();
		}
	}

	return "No country selected";
}