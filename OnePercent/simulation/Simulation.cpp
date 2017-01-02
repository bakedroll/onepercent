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
	: Group()
  , SimulationCallback()
  , m_applySkillsVisitor(new SimulationVisitor(SimulationVisitor::APPLY_SKILLS))
  , m_affectNeighborsVisitor(new SimulationVisitor(SimulationVisitor::AFFECT_NEIGHBORS))
  , m_progressCountriesVisitor(new SimulationVisitor(SimulationVisitor::PROGRESS_COUNTRIES))
  , m_day(0)
{
  m_skillBranches[BRANCH_CONTROL]  = new SkillBranch(BRANCH_CONTROL);
  m_skillBranches[BRANCH_BANKS]    = new SkillBranch(BRANCH_BANKS);
  m_skillBranches[BRANCH_CONCERNS] = new SkillBranch(BRANCH_CONCERNS);
  m_skillBranches[BRANCH_MEDIA]    = new SkillBranch(BRANCH_MEDIA);
  m_skillBranches[BRANCH_POLITICS] = new SkillBranch(BRANCH_POLITICS);

  setUpdateCallback(new Callback());
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

		ref_ptr<CountryData> country = new CountryData(
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

    for (int j = 0; j < NUM_SKILLBRANCHES; j++)
      country->addChild(m_skillBranches[j]);

    m_globeModel->addCountry(int(id), country, triangles);
    addChild(country);

		delete[] name_p;
	}

  CountryMesh::Map& countries = m_globeModel->getCountryMeshs();
  for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
	{
		NeighborList neighborList = neighborMap.find(it->second->getCountryData()->getId())->second;

		for (NeighborList::iterator nit = neighborList.begin(); nit != neighborList.end(); ++nit)
		{
      it->second->addNeighbor(m_globeModel->getCountryMesh(*nit), new NeighborCountryInfo());
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

    osg::ref_ptr<PropertyArray> arr = PropertiesManager::getInstance()->root()->group("skills")->array("passive")->array(i, "attributes");
    int arrsize = arr->size();

    BranchType type = branch_getTypeFromString(typeStr);
    ref_ptr<Skill> skill = new Skill(name);

    for (int j = 0; j < arrsize; j++)
    {
      string valuetypeStr = arr->property<string>(j, "valuetype")->get();
      string methodStr = arr->property<string>(j, "method")->get();
      float value = arr->property<float>(j, "value")->get();
      bool branchAttr = arr->property<bool>(j, "branch_attr")->get();

      if (branchAttr)
      {
        skill->addBranchAttribute(
          type,
          countryValue_getTypeFromString(valuetypeStr),
          valueMethod_getTypeFromString(methodStr),
          value);
      }
      else
      {
        skill->addAttribute(
          countryValue_getTypeFromString(valuetypeStr),
          valueMethod_getTypeFromString(methodStr),
          value);
      }
    }

    m_skillBranches[type]->addSkill(skill);
	}
}

int Simulation::getNumSkills()
{
  int nSkills = 0;
  for (SkillBranch::Map::iterator it = m_skillBranches.begin(); it != m_skillBranches.end(); ++it)
    nSkills += it->second->getNumSkills();

	return nSkills;
}

SkillBranch::Ptr Simulation::getSkillBranch(BranchType type)
{
  return m_skillBranches[type];
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
  accept(*m_applySkillsVisitor);
  accept(*m_affectNeighborsVisitor);
  accept(*m_progressCountriesVisitor);

  m_day++;
}

bool Simulation::callback(SimulationVisitor* visitor)
{
  return true;
}
