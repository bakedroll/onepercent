#pragma once

#include "scripting/LuaObjectMapper.h"
#include "simulation/LuaValuesTable.h"

namespace onep
{
  class LuaBranchValuesTable : public LuaObjectMapper
  {
  public:
    using BranchValuesMap = std::map<std::string, LuaValuesTable::ValuesMap>;

    explicit LuaBranchValuesTable(const luabridge::LuaRef& object);
    ~LuaBranchValuesTable();

    BranchValuesMap getBranchValuesMap();
    LuaValuesTable::Ptr getBranch(const std::string& name) const;
  };
}