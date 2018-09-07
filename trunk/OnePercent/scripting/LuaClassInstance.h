#pragma once

#include "scripting/LuaClass.h"

#include <string>

namespace onep
{
  class LuaClassInstance : public LuaClass
  {
  public:
    virtual ~LuaClassInstance() = default;
    virtual std::string instanceVariableName() = 0;

  };
}
