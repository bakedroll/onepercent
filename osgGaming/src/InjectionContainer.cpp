#include "osgGaming/InjectionContainer.h"
#include "osgGaming/LogManager.h"

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
      {
        char buffer[256];
        sprintf_s(buffer, "%s has %d references left", it->first.name(), refcount - 1);

        OSGG_LOG_WARN(std::string(buffer));
      }
    }
#endif

    m_registeredClasses.clear();
    m_registeredSingletons.clear();


  }
}