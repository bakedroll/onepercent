#pragma once

#include <functional>
#include <QObject>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  namespace Helper
  {
    qint64 measureMsecs(std::function<void()> task);

    template <typename T>
    T parseOsgVecFromLua(const luabridge::LuaRef& table)
    {
      T vec;
      for (auto i=0; i<T::num_components; i++)
      {
        vec._v[i] = table[i + 1];
      }

      return vec;
    }
  }
}