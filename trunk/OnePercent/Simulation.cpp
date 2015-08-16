#include "Simulation.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/ByteStream.h>
#include <osgGaming/Helper.h>
#include <osg/Image>
#include <osgGaming/PropertiesManager.h>

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
		string name = name_p;
		
		float population = stream.read<float>();
		float wealth = float(stream.read<int>());
		unsigned char id = stream.read<unsigned char>();
		float centerX = stream.read<float>();
		float centerY = stream.read<float>();
		float width = stream.read<float>();
		float height = stream.read<float>();

		ref_ptr<Country> country = new Country(
			name,
			id,
			population,
			wealth,
			Vec2f((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI),
			Vec2f(width, height));

		_countries.insert(CountryMap::value_type(id, country));

		delete[] name_p;
	}

	int mapWidth = stream.read<int>();
	int mapHeight = stream.read<int>();


	// dummy neighbors
	ref_ptr<Country> germany = getCountry(20);

	ref_ptr<Country> benelux = getCountry(10);
	ref_ptr<Country> france = getCountry(25);
	ref_ptr<Country> switzerland = getCountry(83);
	ref_ptr<Country> austria = getCountry(70);
	ref_ptr<Country> czech = getCountry(99);
	ref_ptr<Country> poland = getCountry(77);
	ref_ptr<Country> denmark = getCountry(19);

	germany->addNeighborCountry(benelux, new NeighborCountryInfo());
	germany->addNeighborCountry(france, new NeighborCountryInfo());
	germany->addNeighborCountry(switzerland, new NeighborCountryInfo());
	germany->addNeighborCountry(austria, new NeighborCountryInfo());
	germany->addNeighborCountry(czech, new NeighborCountryInfo());
	germany->addNeighborCountry(poland, new NeighborCountryInfo());
	germany->addNeighborCountry(denmark, new NeighborCountryInfo());
	// ###

	_countriesMap = new CountriesMap(mapWidth, mapHeight, reinterpret_cast<unsigned char*>(&bytes[stream.getPos()]));

	ResourceManager::getInstance()->clearCacheResource(countriesBinFilename);
}

void Simulation::loadSkillsXml(string filename)
{
	PropertiesManager::getInstance()->loadPropertiesFromXmlResource(filename);

	int elements = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->size();
	for (int i = 0; i < elements; i++)
	{
		string name = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->property<string>(i, "name")->get();

		printf("SKILL LOADED: %s\n", name.c_str());
	}
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
		return nullptr;
	}

	return _countries.find(id)->second;
}

unsigned char Simulation::getCountryId(Vec2f coord)
{
	Vec2i mapSize = _countriesMap->getSize();

	int ix = int(coord.x() * float(mapSize.x()));
	int iy = int(coord.y() * float(mapSize.y()));

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
	for (CountryMap::iterator it = _countries.begin(); it != _countries.end(); ++it)
	{
		it->second->step();
	}

	_day++;
}

string fillString(string s, int l, bool rightAligned = false)
{
	int fs = l - s.size();

	for (int i = 0; i < fs; i++)
	{
		if (rightAligned)
		{
			s = " " + s;
		}
		else
		{
			s += " ";
		}
	}

	return s;
}

string str(float value, int round = 2)
{
	char roundBuffer[4];
	sprintf(&roundBuffer[0], "%d", round);

	string format = "%." + string(roundBuffer) + "f";

	char buffer[16];

	sprintf(&buffer[0], format.c_str(), value);

	return buffer;
}

string progBar(float value, int length = 12)
{
	string result = "[";

	int l = length - 2;
	int p = l * value;

	for (int i = 0; i < l; i++)
	{
		result += ((i + 1) <= p) ? "|" : " ";
	}

	result += "]";

	return result;
}

void Simulation::printStats(bool onlyActivated)
{
	printf("\n=========================================\n\n");

	printf("%s | %s | %s | %s | %s\n", fillString("Country", 22).c_str(), fillString("Wealth", 8).c_str(), fillString("Anger/Balance", 25).c_str(), fillString("Dept/Relative/Balance", 37).c_str(), fillString("Skills", 19).c_str());
	printf("------------------------------------------------------------------------------------------------------------\n\n");

	for (CountryMap::iterator it = _countries.begin(); it != _countries.end(); ++it)
	{
		if (!onlyActivated || it->second->anySkillBranchActivated())
		{
			string skills = "";

			for (int i = 0; i < Country::SkillBranchCount; i++)
			{
				skills += string("[") + string(it->second->getSKillBranchActivated(Country::SkillBranchType(i)) ? "x" : " ") + string("] ");
			}

			printf("%s | %s | %s | %s | %s\n",
				fillString(it->second->getCountryName(), 22).c_str(),
				fillString(str(it->second->getWealth(), 0), 8, true).c_str(),
				(fillString(str(it->second->getAnger()), 6, true) + " " + progBar(it->second->getAnger()) + fillString(str(it->second->getAngerBalance()), 6, true)).c_str(),
				(fillString(str(it->second->getDept(), 0), 8, true) + fillString(str(it->second->getRelativeDept()), 6, true) + " " + progBar(it->second->getRelativeDept()) + fillString(str(it->second->getDeptBalance(), 1), 10, true)).c_str(),
				skills.c_str());
		}
	}

	printf("\n=========================================\n");
}