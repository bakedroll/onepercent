#pragma once

#include "simulation/SimulationState.h"

#include <QThread>
#include <memory>

namespace onep
{
  class UpdateThread : public QThread
  {
    Q_OBJECT

  public:
    UpdateThread();
    ~UpdateThread();

    void setUpdateFunctions(LuaRefPtr refUpdate_skills_func, LuaRefPtr refUpdate_branches_func);

    virtual void run() override;

    void doNextStep();
    void executeLuaTask(const std::function<void()>& task);
    void shutdown();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  signals:
    void nextStep();
    void onStateUpdated();

  };
}