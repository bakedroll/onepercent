#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/Observable.h>

#define DEF_INT_OBSERVABLE(name) \
  class name : public osgGaming::InitializedObservable<int, 0> \
  { \
  public: \
    name(osgGaming::Injector& injector) \
      : osgGaming::InitializedObservable<int, 0>() \
    {} \
  };

namespace onep
{
  DEF_INT_OBSERVABLE(ONumSkillPoints);
  DEF_INT_OBSERVABLE(ODay);
}