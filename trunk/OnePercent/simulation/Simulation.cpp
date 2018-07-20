#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Multithreading.h"
#include "core/Observables.h"
#include "core/Macros.h"
#include "simulation/SkillsContainer.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationStateReaderWriter.h"
#include "simulation/CountryState.h"
#include "simulation/SkillBranch.h"
#include "simulation/UpdateThread.h"

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
      , oRunning(new osgGaming::Observable<bool>(false))
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
    LuaRefPtr refUpdate_Tick_func;

    osgGaming::Observable<bool>::Ptr oRunning;

    std::vector<osgGaming::Observer<bool>::Ptr> notifyActivatedList;

    UpdateThread thread;

    bool bSyncing;

    LuaRefPtr getLuaFunction(const char* path)
    {
      LuaRefPtr ptr = MAKE_LUAREF_PTR(lua->getObject(path));

      if (!ptr->isFunction())
        OSGG_QLOG_FATAL(QString("Could not load lua function '%1'").arg(path));

      return ptr;
    }
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

    // DirectConnection: Code is executed in UpdateThread
    QConnectFunctor::connect(&m->thread, SIGNAL(onStateUpdated()), [this]()
    {
      // synchronize with lua state
      // Caution: lua state mutex is locked here
      m->bSyncing = true;
      m->stateContainer->accessState([](SimulationState::Ptr state){ state->read(); });
      m->bSyncing = false;

      // increment day
      Multithreading::uiExecuteOrAsync([this](){ m->oDay->set(m->oDay->get() + 1); });
    }, nullptr, Qt::DirectConnection);
  }

  Simulation::~Simulation()
  {
  }

  void Simulation::prepare()
  {
    m->refUpdate_skills_func = m->getLuaFunction("control.update_skills_func");
    m->refUpdate_branches_func = m->getLuaFunction("control.update_branches_func");
    m->refUpdate_Tick_func = m->getLuaFunction("control.update_tick_func");

    int nBranches = m->skillsContainer->getNumBranches();

    m->stateContainer->accessState([this, nBranches](SimulationState::Ptr state)
    {
      for(auto& it : state->getCountryStates())
      {
        CountryState::Ptr cstate = it.second;

        for (int i = 0; i < nBranches; i++)
        {
          std::string branchName = m->skillsContainer->getBranchByIndex(i)->getBranchName();

          m->notifyActivatedList.push_back(cstate->getOActivatedBranch(branchName.c_str())->connect(osgGaming::Func<bool>([this, cstate](bool activated)
          {
            // cancel when already written
            if (m->bSyncing)
              return;

            // We are in a scheduled Lua task already
            m->thread.executeLockedLuaState([&cstate](){ cstate->writeBranchesActivated(); });
          })));
        }
      }
    });


    for (int i = 0; i < nBranches; i++)
    {
      SkillBranch::Ptr branch = m->skillsContainer->getBranchByIndex(i);
     
      int nskills = branch->getNumSkills();
      for (int j = 0; j < nskills; j++)
      {
        Skill::Ptr skill = branch->getSkillByIndex(j);
        m->notifyActivatedList.push_back(skill->getObActivated()->connect(osgGaming::Func<bool>([this, skill](bool activated)
        {
          m->thread.executeLockedLuaState([&skill](){ skill->write(); });
        })));
      }
    }

    m->thread.setUpdateFunctions(m->refUpdate_Tick_func, m->refUpdate_skills_func, m->refUpdate_branches_func);
    m->thread.start();
  }

  void Simulation::shutdownUpdateThread()
  {
    m->thread.shutdown();
    m->thread.doNextStep();
    m->thread.wait();
  }

  UpdateThread* Simulation::getUpdateThread()
  {
    return &m->thread;
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
    m->oRunning->set(true);
    m->timer.start();
    OSGG_LOG_DEBUG("Simulation started");
  }

  void Simulation::stop()
  {
    m->timer.stop();
    m->oRunning->set(false);
    OSGG_LOG_DEBUG("Simulation stopped");
  }

  bool Simulation::running() const
  {
    return m->timer.isActive();
  }

  osgGaming::Observable<bool>::Ptr Simulation::getORunning() const
  {
    return m->oRunning;
  }

  void Simulation::saveState(const std::string& filename)
  {
    SimulationStateReaderWriter rw;
    rw.saveState(filename, m->stateContainer, m->skillsContainer, m->oDay, m->oNumSkillPoints);
  }

  void Simulation::loadState(const std::string& filename)
  {
    SimulationStateReaderWriter rw;

    m->bSyncing = true;
    rw.loadState(filename, m->stateContainer, m->skillsContainer, m->oDay, m->oNumSkillPoints, &m->thread);
    m->bSyncing = false;
  }

  void Simulation::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<Simulation>("Simulation")
      .addFunction("start", &Simulation::lua_start)
      .addFunction("stop", &Simulation::lua_stop)
      .addFunction("set_day", &Simulation::lua_set_day)
      .addFunction("set_interval", &Simulation::lua_set_interval)
      .addFunction("set_skill_points", &Simulation::lua_set_skill_points)
      .addFunction("add_skill_points", &Simulation::lua_add_skill_points)
    .endClass();
  }

  std::string Simulation::instanceVariableName()
  {
    return "simulation";
  }

  void Simulation::lua_start(lua_State* state)
  {
    Multithreading::uiExecuteOrAsync([this](){ start(); });
  }

  void Simulation::lua_stop(lua_State* state)
  {
    Multithreading::uiExecuteOrAsync([this](){ stop(); });
  }

  void Simulation::lua_set_skill_points(int points)
  {
    OSGG_QLOG_DEBUG(QString("Set skill points: %1").arg(points));
    Multithreading::uiExecuteOrAsync([=](){ m->oNumSkillPoints->set(points); });
  }

  void Simulation::lua_add_skill_points(int points)
  {
    OSGG_QLOG_DEBUG(QString("Add skill points: %1").arg(points));
    Multithreading::uiExecuteOrAsync([=]()
    {
      int p = m->oNumSkillPoints->get();
      m->oNumSkillPoints->set(p + points);
    });
  }

  void Simulation::lua_set_day(int day)
  {
    OSGG_QLOG_DEBUG(QString("Set day: %1").arg(day));
    Multithreading::uiExecuteOrAsync([=](){ m->oDay->set(day); });
  }

  void Simulation::lua_set_interval(int interval)
  {
    OSGG_QLOG_DEBUG(QString("Set interval: %1").arg(interval));
    Multithreading::uiExecuteOrAsync([=](){ m->timer.setInterval(interval); });
  }
}