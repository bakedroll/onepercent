#pragma once

#include <QThread>
#include <memory>
#include <functional>

namespace onep
{
  class UpdateThread : public QThread
  {
    Q_OBJECT

  public:
    UpdateThread();
    ~UpdateThread();

    void onTick(std::function<void()> func);

    virtual void run() override;

    void doNextStep();
    void executeLockedTick(const std::function<void()>& task);
    void shutdown();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  signals:
    void nextStep();

  };
}