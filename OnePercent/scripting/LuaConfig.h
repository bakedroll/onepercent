#pragma once

#include "scripting/LuaClassInstance.h"

#include <osgGaming/Macros.h>

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
  class LuaConfig : public osg::Referenced, public LuaClassInstance
  {
  public:
    explicit LuaConfig(osgGaming::Injector& injector);
    ~LuaConfig();

    virtual void registerClass(lua_State* state) override;

    template<typename T>
    T getNumber(const std::string& name)
    {
      return (T)std::dynamic_pointer_cast<ValueType<T>>(
        getValuePtr(name, [this](luabridge::LuaRef& ref)
      {
        return std::make_shared<ValueType<T>>((T)ref);
      }))->value;
    }

    template<typename T>
    T& getVector(const std::string& name)
    {
      return std::dynamic_pointer_cast<ValueType<T>>(
        getValuePtr(name, [this](luabridge::LuaRef& ref)
      {
        return std::make_shared<ValueType<T>>(refToVec<T>(ref));
      }))->value;
    }

    void clearCache();

    void luaExtend(luabridge::LuaRef ref);

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
      std::function<std::shared_ptr<ValueTypeBase>(luabridge::LuaRef& ref)> getFunc);

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