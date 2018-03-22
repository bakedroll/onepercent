#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Observables.h"

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
      , applySkillsVisitor(new SimulationVisitor(SimulationVisitor::APPLY_SKILLS))
      , affectNeighborsVisitor(new SimulationVisitor(SimulationVisitor::AFFECT_NEIGHBORS))
      , progressCountriesVisitor(new SimulationVisitor(SimulationVisitor::PROGRESS_COUNTRIES))
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
    {}

    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager;
    osg::ref_ptr<LuaStateManager> lua;

    SkillBranch::Map skillBranches;

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
    m->skillBranches[BRANCH_CONTROL] = new SkillBranch(BRANCH_CONTROL);
    m->skillBranches[BRANCH_BANKS] = new SkillBranch(BRANCH_BANKS);
    m->skillBranches[BRANCH_CONCERNS] = new SkillBranch(BRANCH_CONCERNS);
    m->skillBranches[BRANCH_MEDIA] = new SkillBranch(BRANCH_MEDIA);
    m->skillBranches[BRANCH_POLITICS] = new SkillBranch(BRANCH_POLITICS);

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

      osg::ref_ptr<osgGaming::PropertyArray> arr = m->propertiesManager->root()->group("skills")->array("passive")->array(i, "attributes");
      int arrsize = arr->size();

      BranchType type = branch_getTypeFromString(typeStr);
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

      m->skillBranches[type]->addSkill(skill);
    }
  }

  void Simulation::attachCountries(CountryMesh::Map& countries)
  {
    for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      for (int j = 0; j < NUM_SKILLBRANCHES; j++)
        it->second->getCountryData()->addChild(m->skillBranches[j]);

      addChild(it->second->getCountryData());
    }
  }

  SkillBranch::Ptr Simulation::getSkillBranch(BranchType type)
  {
    return m->skillBranches[type];
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
      .addFunction("add_branches", &Simulation::lua_add_branches)
    .endClass();
  }

  std::string Simulation::instanceVariableName()
  {
    return "simulation";
  }

  void Simulation::lua_add_branches(lua_State* state)
  {
    luaL_checktype(state, -1, LUA_TTABLE);

    lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
      luaL_checktype(state, -1, LUA_TTABLE);
      lua_getfield(state, -1, "name");

      const char* name = luaL_checkstring(state, -1);
      printf("Branch: %s\n", name);

      lua_pop(state, 2);
    }
  }
}