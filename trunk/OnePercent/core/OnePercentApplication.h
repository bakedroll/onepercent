#pragma once

#include "QtGameApplication.h"

namespace onep
{
  class OnePercentApplication : public QtGameApplication
  {
  public:
    OnePercentApplication(int argc, char** argv);

    void initialize();

    int run();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}