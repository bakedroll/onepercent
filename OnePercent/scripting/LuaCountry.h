#pragma once

#include "scripting/LuaObjectMapper.h"

#include <map>
#include <vector>

namespace onep
{
  class LuaCountry : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaCountry> Ptr;
    typedef std::map<int, Ptr> Map;

    explicit LuaCountry(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaCountry();

    int getId() const;
    std::string getName() const;
    std::vector<int>& getNeighbourIds() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}