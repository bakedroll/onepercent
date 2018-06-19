#pragma once

#include "core/Macros.h"

#include <osg/Referenced>

#include <osgGaming/Injector.h>

#include <functional>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class ConfigManager : public osg::Referenced
  {
  public:
    explicit ConfigManager(osgGaming::Injector& injector);
    ~ConfigManager();

    template<typename T>
    T getNumber(const std::string& name)
    {
      return (T)std::dynamic_pointer_cast<ValueType<float>>(getValuePtr(name, [this](luabridge::LuaRef& ref) { return nullptr; }))->value;
    }

    template<typename T>
    T& getVector(const std::string& name)
    {
      return std::dynamic_pointer_cast<ValueType<T>>(getValuePtr(name, [this](luabridge::LuaRef& ref) { return std::make_shared<ValueType<T>>(refToVec<T>(ref)); }))->value;
    }

    void clearCache();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

    struct ValueTypeBase
    {
      ValueTypeBase() = default;
      virtual ~ValueTypeBase() = default;
    };

    template<typename T>
    struct ValueType : ValueTypeBase
    {
      ValueType(const T& v) : ValueTypeBase(), value(v) {}
      T value;
    };

    std::shared_ptr<ValueTypeBase> getValuePtr(
      const std::string& name,
      std::function<std::shared_ptr<ValueTypeBase>(luabridge::LuaRef& ref)> vecFunc);

    template<typename T>
    T refToVec(luabridge::LuaRef& ref)
    {
      T vec;
      assert_return(ref.isTable(), vec);

      int i = 0;
      for (luabridge::Iterator it(ref); !it.isNil(); ++it)
      {
        luabridge::LuaRef val = *it;
        assert_return(val.isNumber(), vec);
        vec._v[i++] = (float)val;
      }
      
      return vec;
    }

  };
}