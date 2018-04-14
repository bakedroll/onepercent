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
      QElapsedTimer timerSync;
      QElapsedTimer timerOverall;

      long skillsElapsed = 0;
      long long branchesElapsed = 0;

      timerOverall.start();

      LuaStateManager::safeExecute([&]()
      {
        QMutexLocker lock(&m->mutexState);

        timerSkillsUpdate.start();
        (*m->refUpdate_skills_func)();
        skillsElapsed = timerSkillsUpdate.elapsed();

        timerBranchesUpdate.start();
        (*m->refUpdate_branches_func)();
        branchesElapsed = timerBranchesUpdate.elapsed();
      });

      timerSync.start();
      emit onStateUpdated();

      long syncElapsed = timerSync.elapsed();
      long overallElapsed = timerOverall.elapsed();

      OSGG_QLOG_DEBUG(QString("SkillsUpdate: %1ms BranchesUpdate: %2ms Sync: %3ms Overall: %4ms")
        .arg(skillsElapsed)
        .arg(branchesElapsed)
        .arg(syncElapsed)
        .arg(overallElapsed));
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

  QMutex& UpdateThread::getStateMutex() const
  {
    return m->mutexState;
  }
}