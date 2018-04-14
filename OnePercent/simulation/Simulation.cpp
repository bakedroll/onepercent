#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Observables.h"
#include "core/Macros.h"
#include "simulation/SkillsContainer.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/CountryState.h"
#include "simulation/SkillBranch.h"
#include "simulation/UpdateThread.h"

#include <osgGaming/ResourceManager.h>

#include <QTimer>

extern "C"
{
#include <lua.h>
}

#include <LuaBridge.h>

namespace onep
{
  struct Simulation::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , stateContainer(injector.inject<SimulationStateContainer>())
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
      , bSyncing(false)
    {}

    osg::ref_ptr<LuaStateManager> lua;

    SkillsContainer::Ptr skillsContainer;
    SimulationStateContainer::Ptr stateContainer;

    QTimer timer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    LuaRefPtr refUpdate_skills_func;
    LuaRefPtr refUpdate_branches_func;
    LuaRefPtr refDump_object;

    std::vector<osgGaming::Observer<bool>::Ptr> notifyActivatedList;

    UpdateThread thread;

    bool bSyncing;
  };

  Simulation::Simulation(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance()
    , m(new Impl(injector))

  {
    // start with 50 skill points
    m->oNumSkillPoints->set(50);

    m->timer.setSingleShot(false);
    m->timer.setInterval(1000);

    QConnectFunctor::connect(&m->timer, SIGNAL(timeout()), [this]()
    {
      m->thread.doNextStep();
    });

    QConnectFunctor::connect(&m->thread, SIGNAL(onStateUpdated()), [this]()
    {
      QMutexLocker lock(&m->thread.getStateMutex());

      // synchronize with lua state
      m->bSyncing = true;
      m->stateContainer->getState()->read();
      m->bSyncing = false;

      // increment day
      m->oDay->set(m->oDay->get() + 1);
    });
  }

  Simulation::~Simulation()
  {
  }

  void Simulation::prepare()
  {
    m->refUpdate_skills_func    = MAKE_LUAREF_PTR(m->lua->getObject("core.control.update_skills_func"));
    m->refUpdate_branches_func  = MAKE_LUAREF_PTR(m->lua->getObject("core.control.update_branches_func"));
    m->refDump_object           = MAKE_LUAREF_PTR(m->lua->getObject("core.helper.dump_object"));

    if (!m->refUpdate_skills_func->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("update_skills_func"));

    if (!m->refUpdate_branches_func->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("update_branches_func"));

    if (!m->refDump_object->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("dump_object"));

    int nBranches = m->skillsContainer->getNumBranches();

    ONEP_FOREACH(CountryState::Map, it, m->stateContainer->getState()->getCountryStates())
    {
      CountryState::Ptr cstate = it->second;

      for (int i = 0; i < nBranches; i++)
      {
        std::string branchName = m->skillsContainer->getBranchByIndex(i)->getBranchName();

        m->notifyActivatedList.push_back(cstate->getOActivatedBranch(branchName.c_str())->connect(osgGaming::Func<bool>([=](bool activated)
        {
          if (m->bSyncing)
            return;

          QMutexLocker lock(&m->thread.getStateMutex());
          cstate->writeBranchesActivated();
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
        m->notifyActivatedList.push_back(skill->getObActivated()->connect(osgGaming::Func<bool>([=](bool activated)
        {
          QMutexLocker lock(&m->thread.getStateMutex());
          skill->write();
        })));
      }
    }

    m->thread.setUpdateFunctions(m->refUpdate_skills_func, m->refUpdate_branches_func);
    m->thread.start();
  }

  void Simulation::shutdownUpdateThread()
  {
    m->thread.shutdown();
    m->thread.doNextStep();
    m->thread.wait();
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