#pragma once

#include "scripting/LuaClassDefinition.h"

namespace onep
{
    class LuaExternalClassDefinitions : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };
}
