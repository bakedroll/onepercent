#pragma once

#include <luaHelper/LuaBridgeDefinition.h>

namespace onep
{
    class LuaExternalClassDefinitions : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };
}
