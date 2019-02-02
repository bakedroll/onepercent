#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/Observable.h>

#define DEF_OBSERVABLE(name, type, init) \
  class name : public osgGaming::InitializedObservable<type, init> \
    { \
  public: \
    name(osgGaming::Injector& injector) \
      : osgGaming::InitializedObservable<type, init>() \
        {} \
    };

#define DEF_INT_OBSERVABLE(name)    DEF_OBSERVABLE(name, int, 0)

namespace onep
{
  DEF_INT_OBSERVABLE(ONumSkillPoints);
  DEF_INT_OBSERVABLE(ODay);
}