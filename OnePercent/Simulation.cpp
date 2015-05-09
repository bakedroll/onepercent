#include "Simulation.h"

#include <osgGaming/ResourceManager.h>
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

	int ncountries;
	int mapWidth, mapHeight;

	memcpy(&ncountries, bytes, sizeof(int));

	int position = sizeof(int);

	for (int i = 0; i < ncountries; i++)
	{
		string name;
		float population;
		int name_length, bip;
		unsigned char r, g, b;

		memcpy(&name_length, &bytes[position], sizeof(int));
		position += sizeof(int);

		char* name_p = new char[name_length + 1];

		memcpy(&name_p[0], &bytes[position], name_length);
		name_p[name_length] = '\0';
		name = string(name_p);

		position += name_length;

		memcpy(&population, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&bip, &bytes[position], sizeof(int));
		position += sizeof(int);

		memcpy(&r, &bytes[position], sizeof(unsigned char));
		position += sizeof(unsigned char);

		memcpy(&g, &bytes[position], sizeof(unsigned char));
		position += sizeof(unsigned char);

		memcpy(&b, &bytes[position], sizeof(unsigned char));
		position += sizeof(unsigned char);

		ref_ptr<Country> country = new Country(name, Vec3i((int)r, (int)g, (int)b), population, bip);
		_countries.push_back(country);

		delete[] name_p;
	}

	memcpy(&mapWidth, &bytes[position], sizeof(int));
	position += sizeof(int);

	memcpy(&mapHeight, &bytes[position], sizeof(int));
	position += sizeof(int);

	_countriesMap = new CountriesMap(mapWidth, mapHeight, (unsigned char*)&bytes[position]);

	ResourceManager::getInstance()->clearCacheResource(countriesBinFilename);
}

Vec3i Simulation::getCountryColor(Vec2f coord)
{
	Vec2i mapSize = _countriesMap->getSize();

	int ix = (int)(coord.x() * (float)mapSize.x());
	int iy = (int)(coord.y() * (float)mapSize.y());

	return _countriesMap->getDataAt(ix, iy);
}

string Simulation::getCountryName(Vec2f coord)
{
	Vec3i data = getCountryColor(coord);

	for (CountryList::iterator it = _countries.begin(); it != _countries.end(); ++it)
	{
		if (data == it->get()->getColor())
		{
			return it->get()->getCountryName();
		}
	}

	return "No country selected";
}