#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaMapTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaMapTable>;

    LuaMapTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaMapTable();

    bool contains(const std::string& key) const;
    void insert(const std::string& key, const luabridge::LuaRef& elem);
    luabridge::LuaRef getElement(const std::string& key) const;

  };
}