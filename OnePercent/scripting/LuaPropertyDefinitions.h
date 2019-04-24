#pragma once

#include "scripting/LuaBridgeDefinition.h"

#include <osgGaming/Injector.h>

namespace onep
{
    class LuaPropertyDefinitions : public LuaBridgeDefinition
    {
    public:
      explicit LuaPropertyDefinitions(osgGaming::Injector& injector);
      virtual ~LuaPropertyDefinitions();

      void registerDefinition(lua_State* state) override;

    private:
      struct Impl;
      std::unique_ptr<Impl> m;
    };
}
