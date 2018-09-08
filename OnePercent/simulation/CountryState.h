#pragma once

#include "simulation/LuaValuesTable.h"
#include "simulation/LuaBranchValuesTable.h"
#include "simulation/LuaBranchesActivatedTable.h"

namespace onep
{
  class CountryState : public LuaObjectMapper
  {
  public:
    typedef std::shared_ptr<CountryState> Ptr;
    typedef std::map<int, Ptr> Map;

    explicit CountryState(const luabridge::LuaRef& object);
    ~CountryState();

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