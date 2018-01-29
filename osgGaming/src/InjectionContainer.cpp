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
}