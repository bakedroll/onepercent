#include "Multithreading.h"

#include <assert.h>

#include <QTimer>
#include <QThread>

namespace onep
{
  class ExceptionContext
  {
  public:
    ExceptionContext(std::function<void()> func) : m_func(func) {}
    virtual ~ExceptionContext() {}

    virtual void start()
    {
      try
      {
        m_func();
      }
      catch (std::exception&)
      {
        m_exception = std::current_exception();
      }
      catch (...)
      {
        m_exception = std::current_exception();
      }
    }

    virtual void rethrow() const
    {
      if (m_exception)
        std::rethrow_exception(m_exception);
    }

    virtual bool nothrow() const
    {
      return !m_exception;
    }

  protected:
    std::function<void()> m_func;
    std::exception_ptr m_exception;

  };

  void uiExecuteAsyncInternal(std::function<void()> func)
  {
    ExceptionContext* context = new ExceptionContext(func);
    QMetaObject::invokeMethod(qApp, "executeInUIThreadAsync", Qt::QueuedConnection,
      Q_ARG(void*, static_cast<void*>(context)));
  }

  bool Multithreading::m_ignoreFurtherEvents = false;
  std::mutex Multithreading::m_ignoreFurtherEventsMutex;

  Multithreading::Multithreading(int& argc, char** argv)
    : QApplication(argc, argv)
  {
    std::lock_guard<std::mutex> guard(m_ignoreFurtherEventsMutex);
    m_ignoreFurtherEvents = false;
  }

  Multithreading::~Multithreading()
  {
  }

  void Multithreading::ignoreAnyFurtherUiEvents()
  {
    {
      std::lock_guard<std::mutex> guard(m_ignoreFurtherEventsMutex);
      m_ignoreFurtherEvents = true;
    }

    QEventLoop loop;
    QTimer::singleShot(0, &loop, SLOT(quit()));
    loop.exec();
  }

  void Multithreading::uiExecute(std::function<void()> func)
  {
    if (isInUIThread())
    {
      func();
    }
    else
    {
      ExceptionContext context(func);
      QMetaObject::invokeMethod(qApp, "executeInUIThread", Qt::BlockingQueuedConnection,
        Q_ARG(void*, static_cast<void*>(&context)));
      context.rethrow();
    }
  }

  void Multithreading::uiExecuteAsync(std::function<void()> func, bool executeInUIThread)
  {
    assert((executeInUIThread || !isInUIThread()) && "Use uiExecuteOrAsync()!");

    if (ignoringAnyFurtherUiEvents())
      return;

    uiExecuteAsyncInternal(func);
  }

  void Multithreading::uiExecuteOrAsync(std::function<void()> func)
  {
    if (isInUIThread())
    {
      func();
    }
    else
    {
      uiExecuteAsync(func);
    }
  }

  bool Multithreading::isInUIThread()
  {
    return !qApp || QThread::currentThread() == qApp->thread();
  }

  void Multithreading::executeInUIThread(void* exceptionContext)
  {
    try
    {
      static_cast<ExceptionContext*>(exceptionContext)->start();
    }
    catch (std::exception&)
    {
      assert(false);
    }
  }

  void Multithreading::executeInUIThreadAsync(void* exceptionContext)
  {
    ExceptionContext* context = static_cast<ExceptionContext*>(exceptionContext);

    try
    {
      context->start();
    }
    catch (std::exception&)
    {
      assert(false);
    }
    assert(context->nothrow());
    delete context;
  }

  bool Multithreading::ignoringAnyFurtherUiEvents()
  {
    std::lock_guard<std::mutex> guard(m_ignoreFurtherEventsMutex);
    return m_ignoreFurtherEvents;
  }

}