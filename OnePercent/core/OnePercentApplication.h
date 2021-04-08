#pragma once

#include <QtOsgBridge/QtGameApplication.h>

namespace onep
{
  class OnePercentApplication : public QtOsgBridge::QtGameApplication
  {
  public:
    OnePercentApplication(int& argc, char** argv);

    int run();

  protected:
    void registerComponents(osgHelper::ioc::InjectionContainer& container) override;
    void initialize(osgHelper::ioc::Injector& injector) override;
    void deinitialize() override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}
