#pragma once

#include "simulation/Simulation.h"

#include <QtOsgBridge/QtGameApplication.h>

namespace onep
{
  class OnePercentApplication : public QtOsgBridge::QtGameApplication
  {
  public:
    OnePercentApplication(int& argc, char** argv);
    ~OnePercentApplication();

    int run();

  protected:
    void registerComponents(osgHelper::ioc::InjectionContainer& container) override;
    void initialize(osgHelper::ioc::Injector& injector) override;

  private:
    Simulation::Ptr m_simulation;

    void loadStylesheets(const QString& path);

  };
}
