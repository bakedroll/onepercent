#pragma once

#include <osgHelper/ioc/Injector.h>
#include <osgHelper/Observable.h>

#define DEF_OBSERVABLE(name, type, init) \
  class name : public osgHelper::InitializedObservable<type, init> \
    { \
  public: \
    name(osgHelper::ioc::Injector& injector) \
      : osgHelper::InitializedObservable<type, init>() \
        {} \
    };

#define DEF_INT_OBSERVABLE(name)    DEF_OBSERVABLE(name, int, 0)

namespace onep
{
  DEF_INT_OBSERVABLE(ONumSkillPoints);
  DEF_INT_OBSERVABLE(ODay);
}