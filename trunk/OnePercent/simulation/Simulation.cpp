#include "Simulation.h"

#include <osgGaming/ResourceManager.h>
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

void Simulation::attachCountries(CountryMesh::Map& countries)
{
  for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
  {
    for (int j = 0; j < NUM_SKILLBRANCHES; j++)
      it->second->getCountryData()->addChild(m_skillBranches[j]);

    addChild(it->second->getCountryData());
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
