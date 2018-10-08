#pragma once

#include "scripting/LuaObjectMapper.h"

#include <memory>

namespace onep
{
  class LuaValueDef : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaValueDef> Ptr;

    explicit LuaValueDef(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaValueDef();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}