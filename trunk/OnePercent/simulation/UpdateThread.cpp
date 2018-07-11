#include "UpdateThread.h"

#include "core/Macros.h"
#include "core/Multithreading.h"

#include <QElapsedTimer>
#include <QMutex>

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
    QMutex mutexTick;

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

      QMutexLocker tickLock(&m->mutexTick);

      QElapsedTimer timerSkillsUpdate;
      QElapsedTimer timerBranchesUpdate;
      QElapsedTimer timerSync;
      QElapsedTimer timerTotal;

      long skillsElapsed = 0;
      long long branchesElapsed = 0;
      long syncElapsed = 0;

      timerTotal.start();

      LuaStateManager::safeExecute([&]()
      {
        QMutexLocker StateLock(&m->mutexState);

        timerSkillsUpdate.start();
        (*m->refUpdate_skills_func)();
        skillsElapsed = timerSkillsUpdate.elapsed();

        timerBranchesUpdate.start();
        (*m->refUpdate_branches_func)();
        branchesElapsed = timerBranchesUpdate.elapsed();

        timerSync.start();
        emit onStateUpdated();
        syncElapsed = timerSync.elapsed();
      });

      long totalElapsed = timerTotal.elapsed();

      OSGG_QLOG_INFO(QString("SkillsUpdate: %1ms BranchesUpdate: %2ms Sync: %3ms Total: %4ms")
        .arg(skillsElapsed)
        .arg(branchesElapsed)
        .arg(syncElapsed)
        .arg(totalElapsed));
    }

    OSGG_LOG_DEBUG("Shutdown update thread");
  }

  void UpdateThread::doNextStep()
  {
    emit nextStep();
  }

  void UpdateThread::executeLockedLuaState(const std::function<void()>& task)
  {
    QMutexLocker lock(&m->mutexState);

    LuaStateManager::safeExecute([&]()
    {
      task();
    });
  }

  void UpdateThread::executeLockedTick(const std::function<void()>& task)
  {
    QMutexLocker lock(&m->mutexTick);
    task();
  }

  void UpdateThread::shutdown()
  {
    QMutexLocker lock(&m->mutexShutdown);
    m->bShutdown = true;
  }

}