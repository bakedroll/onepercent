#pragma once

#include "scripting/LuaObjectMapper.h"

#include <memory>

namespace onep
{
  class LuaValueDef : public LuaObjectMapper
  {
  public:
    enum class Type
    {
      Default,
      Branch
    };

    typedef std::shared_ptr<LuaValueDef> Ptr;

    explicit LuaValueDef(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaValueDef();

    std::string getName() const;
    Type getType() const;
    float getInit() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}