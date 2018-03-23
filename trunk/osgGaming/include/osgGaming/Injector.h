#pragma once

#include "osgGaming/InjectionContainer.h"
#include "osgGaming/LogManager.h"

#ifdef _DEBUG
#include <vector>
#endif

namespace osgGaming
{
  class Injector
  {
#ifdef _DEBUG
  private:
    int m_depth;
    std::vector<std::type_index> m_creationOrder;

    template<typename T>
    void traceInstance()
    {
      std::type_index ti = typeid(T);

      for (std::vector<std::type_index>::iterator it = m_creationOrder.begin(); it != m_creationOrder.end(); ++it)
      {
        if (*it == ti)
          OSGG_LOG_WARN("Circular dependency detected!");
      }

      char buffer[256];
      sprintf_s(buffer, "%*s%s%s", (m_depth)* 2, "", "Injecting ", ti.name());

      OSGG_LOG_INFO(std::string(buffer));
    }

    template<typename T>
    osg::ref_ptr<T> createInstance()
    {
      traceInstance<T>();

      std::type_index ti = typeid(T);
      m_creationOrder.push_back(ti);

      m_depth++;
      osg::ref_ptr<T> instance = new T(*this);
      m_depth--;

      m_creationOrder.pop_back();

      return instance;
    }
#endif

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
        {
#ifdef _DEBUG
          traceInstance<T>();
#endif
          return osg::ref_ptr<T>(dynamic_cast<T*>(ptr.get()));
        }

#ifdef _DEBUG
        osg::ref_ptr<T> tptr = createInstance<T>();
#else
        osg::ref_ptr<T> tptr = new T(*this);
#endif
        singletons[ti] = tptr;
        return tptr;
      }
      
      InjectionContainer::Classes& classes = m_container->classes();
      if (classes.count(ti) > 0)
      {
#ifdef _DEBUG
        return createInstance<T>();
#else
        return new T(*this);
#endif
      }

      assert(false && "Class T was not registered.");
      return nullptr;
    }

  private:
    InjectionContainer* m_container;

  };
}