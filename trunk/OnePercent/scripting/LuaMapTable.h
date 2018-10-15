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

    /*bool contains(const std::string& key) const;
    void insert(const std::string& key, const luabridge::LuaRef& elem);
    luabridge::LuaRef getElement(const std::string& key) const;
    */
   
    
    template<typename KeyType>
    bool contains(const KeyType& key) const
    {
      return !luaref()[key].isNil();
    }

    template<typename KeyType>
    void insert(const KeyType& key, const luabridge::LuaRef& elem)
    {
      luaref()[key] = elem;
    }

    template<typename KeyType>
    luabridge::LuaRef getElement(const KeyType& key) const
    {
      return luaref()[key];
    }
  
  };
}