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
    QMutex mutexTasks;

    LuaRefPtr refUpdate_skills_func;
    LuaRefPtr refUpdate_branches_func;

    std::vector<std::function<void()>> scheduledTasks;

    bool bShutdown;

    void processScheduledTasks()
    {
      QMutexLocker lock(&mutexTasks);

      for (std::vector<std::function<void()>>::iterator it = scheduledTasks.begin(); it != scheduledTasks.end(); ++it)
        (*it)();

      scheduledTasks.clear();
    }
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
      long syncElapsed = 0;

      timerOverall.start();

      LuaStateManager::safeExecute([&]()
      {
        QMutexLocker lock(&m->mutexState);

        m->processScheduledTasks();

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

  void UpdateThread::scheduleLuaTask(const std::function<void()>& task)
  {
    QMutexLocker lock(&m->mutexTasks);
    m->scheduledTasks.push_back(task);
  }

  void UpdateThread::executeLuaTask(const std::function<void()>& task)
  {
    QMutexLocker lock(&m->mutexState);

    LuaStateManager::safeExecute([&]()
    {
      task();
    });
  }

  void UpdateThread::shutdown()
  {
    QMutexLocker lock(&m->mutexShutdown);
    m->bShutdown = true;
  }

}