#pragma once

#include "scripting/LuaValuesTable.h"
#include "scripting/LuaBranchValuesTable.h"
#include "scripting/LuaBranchesActivatedTable.h"

namespace onep
{
  class LuaCountryState : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<LuaCountryState> Ptr;
    typedef std::map<int, Ptr> Map;

    explicit LuaCountryState(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaCountryState();

    LuaValuesTable::ValuesMap getValuesMap() const;
    LuaBranchValuesTable::BranchValuesMap getBranchValuesMap() const;

    std::shared_ptr<LuaValuesTable> getValuesTable() const;
    std::shared_ptr<LuaBranchValuesTable> getBranchValuesTable() const;
    std::shared_ptr<LuaBranchesActivatedTable> getBranchesActivatedTable() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}