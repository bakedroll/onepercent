#pragma once

#include "scripting/LuaClass.h"

#include <string>

namespace onep
{
  class LuaClassInstance : public LuaClass
  {
  public:
    virtual ~LuaClassInstance() = default;

    // TODO: move to constructor parameter
    virtual std::string instanceVariableName() = 0;

  };
}
