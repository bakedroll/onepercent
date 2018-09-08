#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Multithreading.h"
#include "core/Observables.h"
#include "core/Macros.h"
#include "simulation/SkillsContainer.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationStateReaderWriter.h"
#include "scripting/LuaSkillBranch.h"
#include "simulation/UpdateThread.h"

#include <QTimer>
#include <QElapsedTimer>

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
    {}

    osg::ref_ptr<LuaStateManager> lua;

    SkillsContainer::Ptr skillsContainer;
    SimulationStateContainer::Ptr stateContainer;

    QTimer timer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    LuaRefPtr refUpdate_skills_func;
    LuaRefPtr refUpdate_branches_func;
    LuaRefPtr refUpdate_tick_func;

    osgGaming::Observable<bool>::Ptr oRunning;

    UpdateThread thread;

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
    , m(new Impl(injector))

  {
    // start with 50 skill points
    m->oNumSkillPoints->set(50);

    m->timer.setSingleShot(false);
    m->timer.setInterval(1000);

    m->thread.onTick([this]()
    {
      QElapsedTimer timerTickUpdate;
      QElapsedTimer timerSkillsUpdate;
      QElapsedTimer timerBranchesUpdate;
      QElapsedTimer timerSync;
      QElapsedTimer timerTotal;

      qint64 tickElapsed;
      qint64 skillsElapsed = 0;
      qint64 branchesElapsed = 0;
      qint64 syncElapsed = 0;

      timerTotal.start();

      LuaStateManager::safeExecute([&]()
      {
        m->stateContainer->accessState([&](LuaSimulationState::Ptr state)
        {

          timerTickUpdate.start();
          (*m->refUpdate_tick_func)();
          tickElapsed = timerTickUpdate.elapsed();

          timerSkillsUpdate.start();
          (*m->refUpdate_skills_func)();
          skillsElapsed = timerSkillsUpdate.elapsed();

          timerBranchesUpdate.start();
          (*m->refUpdate_branches_func)();
          branchesElapsed = timerBranchesUpdate.elapsed();

          timerSync.start();
          state->traverseElementsUpdate();
          syncElapsed = timerSync.elapsed();

          Multithreading::uiExecuteOrAsync([this]()
          {
            m->oDay->set(m->oDay->get() + 1);
          });
        });
      });

      long totalElapsed = timerTotal.elapsed();

      OSGG_QLOG_INFO(QString("TickUpdate: %1ms SkillsUpdate: %2ms BranchesUpdate: %3ms Sync: %4ms Total: %5ms")
        .arg(tickElapsed)
        .arg(skillsElapsed)
        .arg(branchesElapsed)
        .arg(syncElapsed)
        .arg(totalElapsed));
    });

    QConnectFunctor::connect(&m->timer, SIGNAL(timeout()), [this]()
    {
      m->thread.doNextStep();
    });
  }

  Simulation::~Simulation()
  {
  }

  void Simulation::prepare()
  {
    m->refUpdate_skills_func = m->getLuaFunction("control.update_skills_func");
    m->refUpdate_branches_func = m->getLuaFunction("control.update_branches_func");
    m->refUpdate_tick_func = m->getLuaFunction("control.update_tick_func");

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

  void Simulation::setUpdateTimerInterval(int msecs)
  {
    m->timer.setInterval(msecs);
  }

  void Simulation::saveState(const std::string& filename)
  {
    SimulationStateReaderWriter rw;
    rw.saveState(filename, m->stateContainer, m->skillsContainer, m->oDay, m->oNumSkillPoints);
  }

  void Simulation::loadState(const std::string& filename)
  {
    SimulationStateReaderWriter rw;
    rw.loadState(filename, m->stateContainer, m->skillsContainer, m->oDay, m->oNumSkillPoints, &m->thread);
  }
}