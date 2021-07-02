#pragma once

#include <luaHelper/LuaPropertiesDefinition.h>

#include <osgHelper/ioc/Injector.h>

namespace onep
{
    class OnePercentPropertiesDefinition : public luaHelper::LuaPropertiesDefinition
    {
    public:
      explicit OnePercentPropertiesDefinition(osgHelper::ioc::Injector& injector);
      virtual ~OnePercentPropertiesDefinition();

      void registerDefinition(lua_State* state) override;

    };
}
