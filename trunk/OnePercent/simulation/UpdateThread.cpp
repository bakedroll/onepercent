#include "UpdateThread.h"

#include "core/Macros.h"
#include "core/Multithreading.h"

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
    QMutex mutexTick;
    bool bShutdown;

    std::function<void()> tickFunc;

  };

  UpdateThread::UpdateThread()
    : QThread()
    , m(new Impl())
  {
  }

  UpdateThread::~UpdateThread()
  {
  }

  void UpdateThread::onTick(std::function<void()> func)
  {
    m->tickFunc = func;
  }

  void UpdateThread::run()
  {
    OSGG_LOG_DEBUG("Starting up update thread");

    QEventLoop eventLoop;
    connect(this, &UpdateThread::nextStep, &eventLoop, &QEventLoop::quit);

    while (!m->shallShutdown())
    {
      eventLoop.exec();
      if (m->shallShutdown())
        break;

      QMutexLocker tickLock(&m->mutexTick);
      m->tickFunc();
    }

    OSGG_LOG_DEBUG("Shutdown update thread");
  }

  void UpdateThread::doNextStep()
  {
    emit nextStep();
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