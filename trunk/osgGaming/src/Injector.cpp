#include "osgGaming/Injector.h"

namespace osgGaming
{
  Injector::Injector(InjectionContainer& container)
    : m_container(&container)
  {
#ifdef _DEBUG
    m_depth = 0;
#endif
  }
}