#pragma once

#include "luadoc/DocsGenerator.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace luadoc
{
  class Namespace : public luabridge::Namespace
  {
  protected:
    template <typename T>
    class Class : public luabridge::Namespace::Class<T>
    {
    public:
      Class(const luabridge::Namespace::Class<T>& other)
        : luabridge::Namespace::Class<T>(other)
      {
      }

      template <typename MemFn>
      Class<T>& addFunction(const char* name, MemFn mf)
      {
        DocsGenerator::instance().addCurrentClassFunction<MemFn>(name);
        luabridge::Namespace::Class<T>::addFunction(name, mf);
        return *this;
      }

      template <typename TG, typename TS>
      Class<T>& addProperty(char const* name, TG (T::* get) () const, void (T::* set) (TS))
      {
        DocsGenerator::instance().addCurrentClassProperty<TG>(name, false);
        luabridge::Namespace::Class<T>::addProperty(name, get, set);
        return *this;
      }

      template <typename TG>
      Class<T>& addProperty(char const* name, TG (T::* get) () const)
      {
        DocsGenerator::instance().addCurrentClassProperty<TG>(name, true);
        luabridge::Namespace::Class<T>::addProperty(name, get);
        return *this;
      }

      template <typename TG, typename TS = TG>
      Class<T>& addProperty(char const* name, TG (*get) (T const*), void (*set) (T*, TS) = 0)
      {
        DocsGenerator::instance().addCurrentClassProperty<TG>(name, false);
        luabridge::Namespace::Class<T>::addProperty(name, get, set);
        return *this;
      }

      luadoc::Namespace endClass()
      {
        DocsGenerator::instance().endClass();
        return luadoc::Namespace(luabridge::Namespace::Class<T>::endClass());
      }
    };

  public:
    explicit Namespace(luabridge::Namespace const& other);

    static Namespace getGlobalNamespace(lua_State* L);

    Namespace beginNamespace(const char* name);
    Namespace endNamespace();

    template <typename T>
    Class<T> beginClass(char const* name)
    {
      DocsGenerator::instance().beginClass<T>(name);
      return luadoc::Namespace::Class<T>(luabridge::Namespace::beginClass<T>(name));
    }

    template <class T, class U>
    Class<T> deriveClass(char const* name)
    {
      DocsGenerator::instance().deriveClass<T, U>(name);
      return luadoc::Namespace::Class<T>(luabridge::Namespace::deriveClass<T, U>(name));
    }

  };
}
