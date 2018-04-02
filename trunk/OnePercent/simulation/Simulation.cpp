#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Observables.h"
#include "core/Macros.h"
#include "simulation/SkillsContainer.h"
#include "simulation/SimulatedValuesContainer.h"
#include "simulation/CountryState.h"
#include "simulation/NeighbourshipsContainer.h"

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

#include <QElapsedTimer>

namespace onep
{
  struct Simulation::Impl
  {
    Impl(osgGaming::Injector& injector)
      : propertiesManager(injector.inject<osgGaming::PropertiesManager>())
      , lua(injector.inject<LuaStateManager>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , valuesContainer(injector.inject<SimulatedValuesContainer>())
      , applySkillsVisitor(new SimulationVisitor(SimulationVisitor::APPLY_SKILLS))
      , affectNeighborsVisitor(new SimulationVisitor(SimulationVisitor::AFFECT_NEIGHBORS))
      , progressCountriesVisitor(new SimulationVisitor(SimulationVisitor::PROGRESS_COUNTRIES))
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
      , neighbourshipsContainer(injector.inject<NeighbourshipsContainer>())
    {}

    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager;
    osg::ref_ptr<LuaStateManager> lua;

    SkillsContainer::Ptr skillsContainer;
    SimulatedValuesContainer::Ptr valuesContainer;

    GlobeModel::Ptr globeModel;

    SimulationVisitor::Ptr applySkillsVisitor;
    SimulationVisitor::Ptr affectNeighborsVisitor;
    SimulationVisitor::Ptr progressCountriesVisitor;

    QTimer timer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    std::map<int, std::string> idCountryMap;

    LuaRefPtr refUpdate_skills_func;
    LuaRefPtr refUpdate_branches_func;
    LuaRefPtr refSet_skill_activated;
    LuaRefPtr refSet_branch_activated;

    LuaRefPtr refDump_object;

    SimulationState::Ptr stateCopy;
    NeighbourshipsContainer::Ptr neighbourshipsContainer;

    std::vector<osgGaming::Observer<bool>::Ptr> notifyActivatedList;
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
      SimulationState::Ptr state = m->valuesContainer->getState();

      QElapsedTimer timerSkillsUpdate;
      QElapsedTimer timerBranchesUpdate;
      QElapsedTimer timerOverall;

      try
      {
        timerOverall.start();

        m->stateCopy->overwrite(state);

        timerSkillsUpdate.start();
        (*m->refUpdate_skills_func)(state.get(), m->stateCopy.get());
        long skillsElapsed = timerSkillsUpdate.elapsed();

        state->overwrite(m->stateCopy);

        timerBranchesUpdate.start();
        (*m->refUpdate_branches_func)(state.get(), m->stateCopy.get());
        long branchesElapsed = timerBranchesUpdate.elapsed();

        m->valuesContainer->setState(m->stateCopy);
        m->stateCopy = state;

        long overallElapsed = timerOverall.elapsed();

        OSGG_QLOG_DEBUG(QString("SkillsUpdate: %1ms BranchesUpdate: %2ms Overall: %3ms").arg(skillsElapsed).arg(branchesElapsed).arg(overallElapsed));

      }
      catch (luabridge::LuaException& e)
      {
        OSGG_QLOG_FATAL(QString("Lua Exception: %1").arg(e.what()));
        assert(false);
      }

      // increment day
      m->oDay->set(m->oDay->get() + 1);
    });
  }

  Simulation::~Simulation()
  {
  }

  void Simulation::prepare()
  {
    luabridge::LuaRef core = m->lua->getGlobal("core");
    luabridge::LuaRef control = core["control"];
    luabridge::LuaRef helper = core["helper"];


    m->refUpdate_skills_func = MAKE_LUAREF_PTR(control["update_skills_func"]);
    m->refUpdate_branches_func = MAKE_LUAREF_PTR(control["update_branches_func"]);
    m->refSet_skill_activated = MAKE_LUAREF_PTR(control["set_skill_activated"]);
    m->refSet_branch_activated = MAKE_LUAREF_PTR(control["set_branch_activated"]);

    m->refDump_object = MAKE_LUAREF_PTR(helper["dump_object"]);

    if (!m->refUpdate_skills_func->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("update_skills_func"));

    if (!m->refUpdate_branches_func->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("update_branches_func"));

    if (!m->refSet_skill_activated->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("set_skill_activated"));

    if (!m->refSet_branch_activated->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("set_branch_activated"));

    if (!m->refDump_object->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("dump_object"));

    m->neighbourshipsContainer->prepare();

    int nBranches = m->skillsContainer->getNumBranches();

    ONEP_FOREACH(CountryState::Map, it, m->valuesContainer->getState()->getCountryStates())
    {
      CountryState::Ptr cstate = it->second;
      int cid = it->first;

      for (int i = 0; i < nBranches; i++)
      {
        std::string branchName = m->skillsContainer->getBranchByIndex(i)->getBranchName();

        m->notifyActivatedList.push_back(cstate->getOActivatedBranch(branchName.c_str())->connect(osgGaming::Func<bool>([this, cid, branchName, cstate](bool activated)
        {
          (*m->refSet_branch_activated)(cid, branchName, activated);
        })));
      }
      
    }

    for (int i = 0; i < nBranches; i++)
    {
      SkillBranch::Ptr branch = m->skillsContainer->getBranchByIndex(i);
     
      int nskills = branch->getNumSkills();
      for (int j = 0; j < nskills; j++)
      {
        Skill::Ptr skill = branch->getSkill(j);
        m->notifyActivatedList.push_back(skill->getObActivated()->connect(osgGaming::Func<bool>([skill, this](bool activated)
        {
          (*m->refSet_skill_activated)(skill->getName(), activated);
        })));
      }
    }

    m->stateCopy = m->valuesContainer->getState()->copy();
  }

  const std::map<int, std::string>& Simulation::getIdCountryMap()
  {
    return m->idCountryMap;
  }

  void Simulation::loadSkillsXml(std::string filename)
  {
    m->propertiesManager->loadPropertiesFromXmlResource(filename);

    int nelements = m->propertiesManager->root()->group("skills")->array("passive")->size();
    for (int i = 0; i < nelements; i++)
    {
      std::string name = m->propertiesManager->root()->group("skills")->array("passive")->property<std::string>(i, "name")->get();
      std::string typeStr = m->propertiesManager->root()->group("skills")->array("passive")->property<std::string>(i, "branch")->get();
      int id = m->skillsContainer->getBranchByName(name)->getBranchId();

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

      SkillBranch::Ptr branch = m->skillsContainer->getBranchByName(name);
      assert(branch.valid());

      branch->addSkill(skill);
    }
  }

  void Simulation::attachCountries(CountryMesh::Map& countries)
  {
    int n = m->skillsContainer->getNumBranches();

    for (CountryMesh::Map::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      for (int j = 0; j < n; j++)
        it->second->getCountryData()->addChild(m->skillsContainer->getBranchByIndex(j));

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
      .addFunction("add_countries", &Simulation::lua_add_countries)
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

  void Simulation::lua_add_countries(lua_State* state)
  {
    luaL_checktype(state, -1, LUA_TTABLE);

    lua_pushnil(state);

    while (lua_next(state, -2) != 0)
    {
      luaL_checktype(state, -1, LUA_TTABLE);
      lua_getfield(state, -1, "id");
      lua_getfield(state, -2, "name");

      int id = luaL_checkinteger(state, -2);
      const char* name = luaL_checkstring(state, -1);

      if (m->idCountryMap.count(id))
        OSGG_QLOG_WARN(QString("Country with id %1 (%2) already exists. Overwriting with %3").arg(id).arg(m->idCountryMap[id].c_str()).arg(QString::fromLocal8Bit(name)));

      m->idCountryMap[id] = std::string(name);
      m->valuesContainer->getState()->addCountryState(id, new CountryState());

      OSGG_QLOG_INFO(QString("Country added: %1").arg(name));

      lua_pop(state, 3);
    }
  }
}