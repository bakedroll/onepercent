#include "osgGaming/Injector.h"

namespace osgGaming
{
  Injector::Injector(InjectionContainer& container)
    : m_container(&container)
  {
  }
}