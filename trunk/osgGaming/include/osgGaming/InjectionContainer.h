#pragma once

#include <typeindex>
#include <set>
#include <map>
#include <assert.h>

#include <osg/Referenced>
#include <osg/ref_ptr>

namespace osgGaming
{
  class InjectionContainer
  {
  public:
    typedef std::set<std::type_index> Classes;
    typedef std::map<std::type_index, osg::ref_ptr<osg::Referenced>> Singletons;

    InjectionContainer();

    template<typename T>
    void registerType()
    {
      std::type_index ti = typeid(T);
      assert(m_registeredClasses.count(ti) == 0);

      m_registeredClasses.insert(ti);
    }

    template<typename T>
    void registerSingletonType()
    {
      std::type_index ti = typeid(T);
      assert(m_registeredSingletons.count(ti) == 0);

      m_registeredSingletons[ti] = osg::ref_ptr<osg::Referenced>();
    }

    Classes& classes();
    Singletons& singletons();

  private:
    std::set<std::type_index> m_registeredClasses;
    std::map<std::type_index, osg::ref_ptr<osg::Referenced>> m_registeredSingletons;

  };
}