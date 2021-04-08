#pragma once

#include "scripting/LuaBridgeDefinition.h"
#include "scripting/LuaStaticProperty.h"

#include <osgHelper/ioc/Injector.h>

namespace onep
{
    class LuaPropertyDefinitions : public LuaBridgeDefinition
    {
    public:
      explicit LuaPropertyDefinitions(osgHelper::ioc::Injector& injector);
      virtual ~LuaPropertyDefinitions();

      void registerDefinition(lua_State* state) override;

    private:
      osgHelper::ioc::Injector& m_injector;

      template <typename T>
      void addProperty(lua_State* state, const char* name)
      {
        LuaStaticProperty<T>::set(m_injector.inject<T>().get());
        getGlobalNamespace(state)
          .beginNamespace("lua")
          .addProperty(name, LuaStaticProperty<T>::get)
          .endNamespace();
      }

    };
}
