#pragma once

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
    LuaTable(const luabridge::LuaRef& object, lua_State* luaState);
    virtual ~LuaTable();

    luabridge::LuaRef& luaRef() const;

    bool              getBoolean(const std::string& key) const;
    double            getNumber(const std::string& key) const;
    std::string       getString(const std::string& key) const;
    luabridge::LuaRef getTable(const std::string& key) const;
    luabridge::LuaRef getFunction(const std::string& key) const;
    luabridge::LuaRef getUserData(const std::string& key) const;

    template<typename T>
    std::shared_ptr<T> getCustomData(const std::string& key) const
    {
      return std::make_shared<T>(checkType(getValueRef(key), LUA_TTABLE, key), luaState());
    }

  protected:
    lua_State* luaState() const;

  private: 
    std::unique_ptr<luabridge::LuaRef> m_ref;
    lua_State*                         m_luaState;

    luabridge::LuaRef  getValueRef(const std::string& key) const;
    const luabridge::LuaRef& checkType(const luabridge::LuaRef& ref, int type, const std::string& key) const;

  };

}