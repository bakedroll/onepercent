#pragma once

#include <osgGaming/Injector.h>

#include <memory>

namespace onep
{
  class LuaStateManager : public osg::Referenced
  {
  public:
    explicit LuaStateManager(osgGaming::Injector& injector);
    ~LuaStateManager();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}