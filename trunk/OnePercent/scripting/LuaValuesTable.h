#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaValuesTable : public LuaObjectMapper
  {
  public:
    using ValuesMap = std::map<std::string, float>;
    using Ptr = std::shared_ptr<LuaValuesTable>;

    explicit LuaValuesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaValuesTable();

    ValuesMap getValuesMap();

    float getValue(const std::string& name) const;
    void setValue(const std::string&, float value);

  };
}