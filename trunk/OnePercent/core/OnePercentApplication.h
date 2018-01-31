#pragma once

#include "QtGameApplication.h"

namespace onep
{
  class OnePercentApplication : public QtGameApplication
  {
  public:
    OnePercentApplication(int& argc, char** argv);

    int run();

  protected:
    virtual void registerComponents(osgGaming::InjectionContainer& container) override;
    virtual void initialize(osgGaming::Injector& injector) override;

  };
}
