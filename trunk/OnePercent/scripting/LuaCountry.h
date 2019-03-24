#pragma once

#include "scripting/LuaObjectMapper.h"

#include <map>

namespace onep
{
  class LuaCountry : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaCountry> Ptr;
    typedef std::map<int, Ptr> Map;

    explicit LuaCountry(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaCountry();

    int                getId() const;
    const std::string& getName() const;

    float getInitValue(const std::string& valueName, float defaultValue = 0.0f) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}