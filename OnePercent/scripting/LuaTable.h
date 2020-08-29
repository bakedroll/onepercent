#pragma once

#include <functional>
#include <memory>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{

  class LuaTable
  {
  public:
    using IteratorFunc = std::function<void(luabridge::LuaRef)>;

    LuaTable(const luabridge::LuaRef& object, lua_State* luaState);
    virtual ~LuaTable();

    luabridge::LuaRef& luaRef() const;

    bool hasValue(const std::string& key) const;

    bool                      getBoolean(const std::string& key) const;
    std::string               getString(const std::string& key) const;
    std::shared_ptr<LuaTable> getTable(const std::string& key) const;
    luabridge::LuaRef         getFunction(const std::string& key) const;
    luabridge::LuaRef         getUserData(const std::string& key) const;

    template<typename T>
    T getNumber(const std::string& key) const
    {
        return checkType(getValueRef(key), LUA_TNUMBER, key);
    }

    void iterateValues(int type, IteratorFunc iterFunc) const;

  protected:
    lua_State* luaState() const;

  private: 
    std::unique_ptr<luabridge::LuaRef> m_ref;
    lua_State*                         m_luaState;

    luabridge::LuaRef               getValueRef(const std::string& key) const;
    static const luabridge::LuaRef& checkType(const luabridge::LuaRef& ref, int type, const std::string& key);
  };

}