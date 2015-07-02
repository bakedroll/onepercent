#include "Simulation.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/ByteStream.h>
#include <osgGaming/Helper.h>
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
		float centerX = stream.read<float>();
		float centerY = stream.read<float>();
		float width = stream.read<float>();
		float height = stream.read<float>();

		ref_ptr<Country> country = new Country(
			name,
			id,
			population,
			bip,
			Vec2f((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI),
			Vec2f(width, height));

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

ref_ptr<Country> Simulation::getCountry(Vec2f coord)
{
	unsigned char id = getCountryId(coord);

	if (id == 255)
	{
		return NULL;
	}

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

string fillString(string s, int l)
{
	int fs = l - s.size();

	for (int i = 0; i < fs; i++)
	{
		s += " ";
	}

	return s;
}

void Simulation::printStats()
{
	printf("\n=========================================\n\n");

	char buffer[255];

	for (CountryMap::iterator it = _countries.begin(); it != _countries.end(); ++it)
	{
		string name = fillString(it->second->getCountryName(), 22);

		sprintf(&buffer[0], "%d Mio", it->second->getBip());
		string bip = fillString(buffer, 12);

		string skills = "";

		for (int i = 0; i < Country::SkillBranchCount; i++)
		{
			skills += string("[") + string(it->second->getSKillBranchActivated((Country::SkillBranchType)i) ? "x" : " ") + string("] ");
		}

		printf("%sBip: %s Skills: %s\n", name.data(), bip.data(), skills.data());
	}

	printf("\n=========================================\n");
}