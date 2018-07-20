#pragma once

#include "osgGaming/Injector.h"

namespace osgGaming
{
  template<typename T>
  class AbstractFactory : public osg::Referenced
  {
  public:
    explicit AbstractFactory(Injector& injector) {}
    ~AbstractFactory() = default;

    osg::ref_ptr<T> make() const
    {
      return new T();
    }
  };
}