#include "scripting/LuaClassInstance.h"

namespace onep
{
  LuaClassInstance::LuaClassInstance(const std::string instanceVariableName)
    : m_instanceVariableName(instanceVariableName)
  {
  }

  std::string LuaClassInstance::getInstanceVariableName() const
  {
    return m_instanceVariableName;
  }
}