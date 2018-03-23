#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Observables.h"

#include "SkillBranchContainer.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/PropertiesManager.h>

#include <QTimer>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  struct Simulation::Impl
  {
    Impl(osgGaming::Injector& injector)
      : propertiesManager(injector.inject<osgGaming::PropertiesManager>())
      , lua(injector.inject<LuaStateManager>())
      , skillBranchContainer(injector.inject<SkillBranchContainer>())
      , applySkillsVisitor(new SimulationVisitor(SimulationVisitor::APPLY_SKILLS))
      , affectNeighborsVisitor(new SimulationVisitor(SimulationVisitor::AFFECT_NEIGHBORS))
      , progressCountriesVisitor(new SimulationVisitor(SimulationVisitor::PROGRESS_COUNTRIES))
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager;
    osg::ref_ptr<LuaStateManager> lua;

    SkillBranchContainer::Ptr skillBranchContainer;

    GlobeModel::Ptr globeModel;

    SimulationVisitor::Ptr applySkillsVisitor;
    SimulationVisitor::Ptr affectNeighborsVisitor;
    SimulationVisitor::Ptr progressCountriesVisitor;

    QTimer timer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;
  };

  Simulation::Simulation(osgGaming::Injector& injector)
    : Group()
    , SimulationCallback()
    , m(new Impl(injector))

  {
    // start with 50 skill points
    m->oNumSkillPoints->set(50);

    m->timer.setSingleShot(false);
    m->timer.setInterval(1000);

    setUpdateCallback(new Callback());

    QConnectFunctor::connect(&m->timer, SIGNAL(timeout()), [this]()
    {
      accept(*m->applySkillsVisitor);
      accept(*m->affectNeighborsVisitor);
      accept(*m->progressCountriesVisitor);

      // increment day
      m->oDay->set(m->oDay->get() + 1);
    });
  }

  Simulation::~Simulation()
  {
  }

  void Simulation::loadSkillsXml(std::string filename)
  {
    m->propertiesManager->loadPropertiesFromXmlResource(filename);

    int nelements = m->propertiesManager->root()->group("skills")->array("passive")->size();
    for (int i = 0; i < nelements; i++)
    {
      std::string name = m->propertiesManager->root()->group("skills")->array("passive")->property<std::string>(i, "name")->get();
      std::string typeStr = m->propertiesManager->root()->group("skills")->array("passive")->property<std::string>(i, "branch")->get();
      int id = m->skillBranchContainer->getBranchByName(name)->getBranchId();

      osg::ref_ptr<osgGaming::PropertyArray> arr = m->propertiesManager->root()->group("skills")->array("passive")->array(i, "attributes");
      int arrsize = arr->size();

      osg::ref_ptr<Skill> skill = new Skill(name);

      for (int j = 0; j < arrsize; j++)
      {
        std::string valuetypeStr = arr->property<std::string>(j, "valuetype")->get();
        std::string methodStr = arr->property<std::string>(j, "method")->get();
        float value = arr->property<float>(j, "value")->get();
        bool branchAttr = arr->property<bool>(j, "branch_attr")->get();

        if (branchAttr)
        {
          skill->addBranchAttribute(
            id,
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

      SkillBranch::Ptr branch = m->skillBranchContainer->getBranchByName(name);
      assert(branch.valid());

      branch->addSkill(skill);
    }
  }

  void Simulation::attachCountries(CountryMesh::Map& countries)
  {
    int n = m->skillBranchContainer->getNumBranches();

    for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      for (int j = 0; j < n; j++)
        it->second->getCountryData()->addChild(m->skillBranchContainer->getBranchByIndex(j));

      addChild(it->second->getCountryData());
    }
  }

  bool Simulation::paySkillPoints(int points)
  {
    int amount = m->oNumSkillPoints->get();
    if (amount < points)
      return false;

    m->oNumSkillPoints->set(amount - points);
    return true;
  }

  void Simulation::start()
  {
    m->timer.start();
  }

  void Simulation::stop()
  {
    m->timer.stop();
  }

  bool Simulation::running() const
  {
    return m->timer.isActive();
  }

  bool Simulation::callback(SimulationVisitor* visitor)
  {
    return true;
  }

  void Simulation::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<Simulation>("Simulation")
      .addFunction("start", &Simulation::lua_start)
      .addFunction("stop", &Simulation::lua_stop)
    .endClass();
  }

  std::string Simulation::instanceVariableName()
  {
    return "simulation";
  }

  void Simulation::lua_start(lua_State* state)
  {
    start();
  }

  void Simulation::lua_stop(lua_State* state)
  {
    stop();
  }
}