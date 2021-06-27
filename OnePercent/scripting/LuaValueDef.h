#pragma once

#include "scripting/LuaTableMappedObject.h"

#include <memory>

namespace onep
{
  class LuaValueDef : public LuaTableMappedObject
  {
  public:
    typedef std::shared_ptr<LuaValueDef> Ptr;

    explicit LuaValueDef(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaValueDef();

    std::string getName() const;
    std::string getGroup() const;
    float       getInit() const;
    bool        getIsVisible() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}