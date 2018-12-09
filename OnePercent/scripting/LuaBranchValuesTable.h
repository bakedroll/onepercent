#pragma once

#include "scripting/LuaObjectMapper.h"
#include "scripting/LuaValuesTable.h"

namespace onep
{
  class LuaBranchValuesTable : public LuaObjectMapper
  {
  public:
    using BranchValuesMap = std::map<std::string, LuaValuesTable::ValuesMap>;

    explicit LuaBranchValuesTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaBranchValuesTable();

    void addBranch(const std::string& name);

    BranchValuesMap getBranchValuesMap();
    LuaValuesTable::Ptr getBranch(const std::string& name) const;
  };
}