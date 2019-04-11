#pragma once

#include "scripting/LuaBridgeDefinition.h"

namespace onep
{
    class LuaExternalClassDefinitions : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };
}
