#pragma once

#include "osgGaming/InjectionContainer.h"

namespace osgGaming
{
  class Injector
  {
  public:
    explicit Injector(InjectionContainer& container);

    template<typename T>
    osg::ref_ptr<T> inject()
    {
      std::type_index ti = typeid(T);

      InjectionContainer::Singletons& singletons = m_container->singletons();
      if (singletons.count(ti) > 0)
      {
        osg::ref_ptr<osg::Referenced> ptr = singletons[ti];
        if (ptr.valid())
          return osg::ref_ptr<T>(dynamic_cast<T*>(ptr.get()));

        osg::ref_ptr<T> tptr = new T(*this);
        singletons[ti] = tptr;
        return tptr;
      }
      
      InjectionContainer::Classes& classes = m_container->classes();
      if (classes.count(ti) > 0)
      {
        return new T(*this);
      }

      assert(false && "Class T was not registered.");
      return nullptr;
    }

  private:
    InjectionContainer* m_container;

  };
}