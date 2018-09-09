#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaCountriesTable : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaCountriesTable>;

    explicit LuaCountriesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaCountriesTable();

    void updateNeighbours();

  protected:
    virtual void onTraverse(int type, luabridge::LuaRef& object) override;

  };
}