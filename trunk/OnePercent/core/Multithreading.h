#pragma once

#include <QApplication>
#include <functional>
#include <mutex>

namespace onep
{
  class Multithreading : public QApplication
  {
    Q_OBJECT

  public:
    Multithreading(int argc, char **argv);
    virtual ~Multithreading();

    static void ignoreAnyFurtherUiEvents();
    static void uiExecute(std::function<void()> func);
    static void uiExecuteAsync(std::function<void()> func, bool executeInUIThread = false);
    static void uiExecuteOrAsync(std::function<void()> func);
    static bool isInUIThread();

  public slots:
    void executeInUIThread(void* exceptionContext);
    void executeInUIThreadAsync(void* exceptionContext);

  private:
    static bool ignoringAnyFurtherUiEvents();

    static bool m_ignoreFurtherEvents;
    static std::mutex m_ignoreFurtherEventsMutex;

  };

} // namespace scopis
