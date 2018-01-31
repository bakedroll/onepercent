#include "osgGaming/InjectionContainer.h"

namespace osgGaming
{
  InjectionContainer::InjectionContainer()
  {
  }

  InjectionContainer::Classes& InjectionContainer::classes()
  {
    return m_registeredClasses;
  }

  InjectionContainer::Singletons& InjectionContainer::singletons()
  {
    return m_registeredSingletons;
  }

  void InjectionContainer::clear()
  {
#ifdef _DEBUG
    for (Singletons::iterator it = m_registeredSingletons.begin(); it != m_registeredSingletons.end(); ++it)
    {
      int refcount = it->second->referenceCount();
      if (refcount > 1)
        printf("Warning: %s has %d references left\n", it->first.name(), refcount - 1);
    }
#endif

    m_registeredClasses.clear();
    m_registeredSingletons.clear();


  }
}