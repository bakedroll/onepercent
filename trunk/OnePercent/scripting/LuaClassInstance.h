#pragma once

#include "scripting/LuaClass.h"

#include <string>

namespace onep
{
  class LuaClassInstance : public LuaClass
  {
  public:
    explicit LuaClassInstance(const std::string instanceVariableName);
    virtual ~LuaClassInstance() = default;

    std::string getInstanceVariableName() const;

  private:
    std::string m_instanceVariableName;

  };
}
