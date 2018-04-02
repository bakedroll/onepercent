#include "UpdateThread.h"

#include "core/Macros.h"

#include <QEventLoop>
#include <QElapsedTimer>

namespace onep
{
  struct UpdateThread::Impl
  {
    Impl()
      : bShutdown(false)
    {}

    bool shallShutdown()
    {
      QMutexLocker lock(&mutexShutdown);
      return bShutdown;
    }

    QMutex mutexShutdown;
    QMutex mutexState;

    SimulationState::Ptr state;
    SimulationState::Ptr nextState;

    LuaRefPtr refUpdate_skills_func;
    LuaRefPtr refUpdate_branches_func;

    bool bShutdown;
  };

  UpdateThread::UpdateThread()
    : QThread()
    , m(new Impl())
  {

  }

  UpdateThread::~UpdateThread()
  {
  }

  void UpdateThread::initializeState(SimulationState::Ptr state)
  {
    m->state = state->copy();
    m->nextState = state->copy();
  }

  void UpdateThread::setUpdateFunctions(LuaRefPtr refUpdate_skills_func, LuaRefPtr refUpdate_branches_func)
  {
    m->refUpdate_skills_func = refUpdate_skills_func;
    m->refUpdate_branches_func = refUpdate_branches_func;
  }

  void UpdateThread::run()
  {
    OSGG_LOG_DEBUG("Starting up update thread");

    QEventLoop eventLoop;
    connect(this, SIGNAL(nextStep()), &eventLoop, SLOT(quit()));

    while (!m->shallShutdown())
    {
      eventLoop.exec();
      if (m->shallShutdown())
        break;

      QElapsedTimer timerSkillsUpdate;
      QElapsedTimer timerBranchesUpdate;
      QElapsedTimer timerOverall;

      try
      {
        QMutexLocker lock(&m->mutexState);

        timerOverall.start();

        m->nextState->overwrite(m->state);

        timerSkillsUpdate.start();
        (*m->refUpdate_skills_func)(m->state.get(), m->nextState.get());
        long skillsElapsed = timerSkillsUpdate.elapsed();

        m->state->overwrite(m->nextState);

        timerBranchesUpdate.start();
        (*m->refUpdate_branches_func)(m->state.get(), m->nextState.get());
        long branchesElapsed = timerBranchesUpdate.elapsed();

        m->state.swap(m->nextState);
        long overallElapsed = timerOverall.elapsed();

        OSGG_QLOG_DEBUG(QString("SkillsUpdate: %1ms BranchesUpdate: %2ms Overall: %3ms").arg(skillsElapsed).arg(branchesElapsed).arg(overallElapsed));

      }
      catch (luabridge::LuaException& e)
      {
        OSGG_QLOG_FATAL(QString("Lua Exception: %1").arg(e.what()));
        assert(false);
      }

      emit onStateUpdated();
    }

    OSGG_LOG_DEBUG("Shutdown update thread");
  }

  void UpdateThread::doNextStep()
  {
    emit nextStep();
  }

  void UpdateThread::shutdown()
  {
    QMutexLocker lock(&m->mutexShutdown);
    m->bShutdown = true;
  }

  SimulationState::Ptr UpdateThread::getState() const
  {
    QMutexLocker lock(&m->mutexState);
    return m->state;
  }

  QMutex& UpdateThread::getStateMutex()
  {
    return m->mutexState;
  }
}