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
	  m_day(0)
{
  m_skillBranches[SkillBranch::BRANCH_CONTROL]  = new SkillBranch(SkillBranch::BRANCH_CONTROL);
  m_skillBranches[SkillBranch::BRANCH_BANKS]    = new SkillBranch(SkillBranch::BRANCH_BANKS);
  m_skillBranches[SkillBranch::BRANCH_CONCERNS] = new SkillBranch(SkillBranch::BRANCH_CONCERNS);
  m_skillBranches[SkillBranch::BRANCH_MEDIA]    = new SkillBranch(SkillBranch::BRANCH_MEDIA);
  m_skillBranches[SkillBranch::BRANCH_POLITICS] = new SkillBranch(SkillBranch::BRANCH_POLITICS);
}

void Simulation::loadCountries(std::string filename)
{
	typedef vector<unsigned char> NeighborList;
	typedef map<unsigned char, NeighborList> NeighborMap;

  char* bytes = ResourceManager::getInstance()->loadBinary(filename);

	ByteStream stream(bytes);

	NeighborMap neighborMap;

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

		NeighborList neighborList;

		int neighbors_count = stream.read<int>();
		for (int j = 0; j < neighbors_count; j++)
		{
			neighborList.push_back(stream.read<unsigned char>());
		}

    neighborMap.insert(NeighborMap::value_type(id, neighborList));

    ref_ptr<DrawElementsUInt> triangles = new DrawElementsUInt(GL_TRIANGLES, 0);
    int triangles_count = stream.read<int>();
    for (int j = 0; j < triangles_count; j++)
    {
      int v0 = stream.read<int>();
      int v1 = stream.read<int>();
      int v2 = stream.read<int>();

      triangles->push_back(v0);
      triangles->push_back(v2);
      triangles->push_back(v1);
    }

    m_globeModel->addCountry(int(id), country, triangles);

		delete[] name_p;
	}

  CountryMesh::Map& countries = m_globeModel->getCountryMeshs();
  for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
	{
		NeighborList neighborList = neighborMap.find(it->second->getCountryData()->getId())->second;

		for (NeighborList::iterator nit = neighborList.begin(); nit != neighborList.end(); ++nit)
		{
      it->second->addNeighborCountry(m_globeModel->getCountryMesh(*nit), new NeighborCountryInfo());
		}
	}

	int mapWidth = stream.read<int>();
	int mapHeight = stream.read<int>();

	m_globeModel->setCountriesMap(new CountriesMap(mapWidth, mapHeight, reinterpret_cast<unsigned char*>(&bytes[stream.getPos()])));

  ResourceManager::getInstance()->clearCacheResource(filename);
}

void Simulation::loadSkillsXml(string filename)
{
	PropertiesManager::getInstance()->loadPropertiesFromXmlResource(filename);

	int nelements = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->size();
	for (int i = 0; i < nelements; i++)
	{
		string name = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->property<string>(i, "name")->get();
		string typeStr = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->property<string>(i, "branch")->get();
		float anger = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->property<float>(i, "anger")->get();
		float interest = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->property<float>(i, "interest")->get();
    float propagation = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->property<float>(i, "propagation")->get();

    SkillBranch::Type type = SkillBranch::getTypeFromString(typeStr);
    ref_ptr<Skill> skill = new Skill(name);
		skill->setAnger(anger);
		skill->setInterest(interest);
    skill->setPropagation(propagation);

    m_skillBranches[type]->addSkill(skill);

		printf("SKILL LOADED: %s\n", name.c_str());
	}
}

ref_ptr<Skill> Simulation::getSkill(int id)
{
  int n = 0;
  Skill::Ptr result;
  for (auto& branch : m_skillBranches)
  {
    branch.second->forEachSkill([&n, &id, &result](Skill::Ptr skill)
    {
      if (id == n)
        result = skill;
      n++;
    });
  }

  return result;
}

int Simulation::getNumSkills()
{
	return m_skillBranches.size();
}

int Simulation::getDay()
{
	return m_day;
}

void Simulation::setGlobeModel(GlobeModel::Ptr model)
{
  m_globeModel = model;
}

void Simulation::step()
{
  CountryMesh::Map& countries = m_globeModel->getCountryMeshs();

  for (CountryMesh::Map::iterator itcountry = countries.begin(); itcountry != countries.end(); ++itcountry)
	{
    for (SkillBranch::Map::iterator itbranch = m_skillBranches.begin(); itbranch != m_skillBranches.end(); ++itbranch)
    {
      if (itcountry->second->getCountryData()->getSkillBranchActivated(itbranch->first))
      {
        itbranch->second->forEachActivatedSkill([&itcountry](Skill::Ptr skill)
        {
          skill->takeEffect(itcountry->second->getCountryData());
        });
      }
    }

    CountryMesh::Neighbor::List& neighbors = itcountry->second->getNeighborCountryMeshs();
    for (CountryMesh::Neighbor::List::iterator itneighbor = neighbors.begin(); itneighbor != neighbors.end(); ++itneighbor)
    {
      //itneighbor->mesh->getCountryData()->getAffectedByNeighborValue()
    }

	}

  for (CountryMesh::Map::iterator itcountry = countries.begin(); itcountry != countries.end(); ++itcountry)
    itcountry->second->getCountryData()->step();

	m_day++;
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

/*void Simulation::printStats(bool onlyActivated)
{
	printf("\n=========================================\n\n");

	printf("%s | %s | %s | %s | %s\n", fillString("Country", 22).c_str(), fillString("Wealth", 8).c_str(), fillString("Anger/Balance", 25).c_str(), fillString("Dept/Relative/Balance", 37).c_str(), fillString("Skills", 19).c_str());
	printf("------------------------------------------------------------------------------------------------------------\n\n");

  CountryMesh::Map& countries = m_globeModel->getCountryMeshs();
  for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
	{
		if (!onlyActivated || it->second->getCountryData()->anySkillBranchActivated())
		{
			string skills = "";

			for (int i = 0; i < Country::SkillBranchCount; i++)
			{
        skills += string("[") + string(it->second->getCountryData()->getSKillBranchActivated(Country::SkillBranchType(i)) ? "x" : " ") + string("] ");
			}

			printf("%s | %s | %s | %s | %s\n",
        fillString(it->second->getCountryData()->getCountryName(), 22).c_str(),
        fillString(str(it->second->getCountryData()->getWealth(), 0), 8, true).c_str(),
        (fillString(str(it->second->getCountryData()->getAnger()), 6, true) + " " + progBar(it->second->getCountryData()->getAnger()) + fillString(str(it->second->getCountryData()->getAngerBalance()), 6, true)).c_str(),
        (fillString(str(it->second->getCountryData()->getDept(), 0), 8, true) + fillString(str(it->second->getCountryData()->getRelativeDept()), 6, true) + " " + progBar(it->second->getCountryData()->getRelativeDept()) + fillString(str(it->second->getCountryData()->getDeptBalance(), 1), 10, true)).c_str(),
				skills.c_str());
		}
	}

	printf("\n=========================================\n");
}*/