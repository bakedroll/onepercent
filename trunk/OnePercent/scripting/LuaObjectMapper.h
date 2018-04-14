#pragma once

#include <memory>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class LuaObjectMapper
  {
  public:
    LuaObjectMapper(const luabridge::LuaRef& object);
    virtual ~LuaObjectMapper();

    void write();
    void read();

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const {};
    virtual void readObject(const luabridge::LuaRef& object) {};

  private:
    std::unique_ptr<luabridge::LuaRef> m_ref;

  };

}