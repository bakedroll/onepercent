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

      template <typename U>
      Class<T>& addStaticData(char const* name, U* pu, bool isWritable = true)
      {
        luabridge::Namespace::Class<T>::addStaticData(name, pu, isWritable);
        return *this;
      }

      template <typename U>
      Class<T>& addStaticProperty(char const* name, U (*get)(), void (*set)(U) = 0)
      {
        luabridge::Namespace::Class<T>::addStaticProperty(name, get, set);
        return *this;
      }

      template <typename FP>
      Class<T>& addStaticFunction(char const* name, FP const fp)
      {
        luabridge::Namespace::Class<T>::addStaticFunction(name, fp);
        return *this;
      }

      Class<T>& addStaticCFunction(char const* name, int (*const fp)(lua_State*))
      {
        luabridge::Namespace::Class<T>::addStaticCFunction(name, fp);
        return *this;
      }

      template <typename U>
      Class<T>& addData(char const* name, const U T::* mp, bool isWritable = true)
      {
        luabridge::Namespace::Class<T>::addData(name, mp, isWritable);
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

      template <typename MemFn>
      Class<T>& addFunction(const char* name, MemFn mf)
      {
        DocsGenerator::instance().addCurrentClassFunction<MemFn>(name);
        luabridge::Namespace::Class<T>::addFunction(name, mf);
        return *this;
      }

      Class<T>& addCFunction(char const* name, int (T::*mfp)(lua_State*))
      {
        luabridge::Namespace::Class<T>::addCFunction(name, mfp);
        return *this;
      }

      Class<T>& addCFunction(char const* name, int (T::*mfp)(lua_State*) const)
      {
        luabridge::Namespace::Class<T>::addCFunction(name, mfp);
        return *this;
      }

      template <typename MemFn, typename C>
      Class<T>& addConstructor()
      {
        luabridge::Namespace::Class<T>::template addConstructor<MemFn, C>();
        return *this;
      }

      template <typename MemFn>
      Class<T>& addConstructor()
      {
        luabridge::Namespace::Class<T>::template addConstructor<MemFn>();
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
    Namespace& addVariable(char const* name, T* pt, bool isWritable = true)
    {
      luabridge::Namespace::addVariable(name, pt, isWritable);
      return *this;
    }

    template <typename TG, typename TS = TG>
    Namespace& addProperty(char const* name, TG (*get) (), void (*set)(TS) = 0)
    {
      luabridge::Namespace::addProperty(name, get, set);
      return *this;
    }

    template <typename FP>
    Namespace& addFunction(char const* name, FP const fp)
    {
      luabridge::Namespace::addFunction(name, fp);
      return *this;
    }

    Namespace& addCFunction(char const* name, int (*const fp)(lua_State*))
    {
      luabridge::Namespace::addCFunction(name, fp);
      return *this;
    }

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

  inline luadoc::Namespace getGlobalNamespace(lua_State* L)
  {
    return luadoc::Namespace::getGlobalNamespace(L);
  }
}
