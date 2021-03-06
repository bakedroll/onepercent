#pragma once

#include "scripting/LuaBranchesActivatedTable.h"

#include <luaHelper/LuaValueGroupTable.h>

namespace onep
{
  class LuaCountryState : public luaHelper::LuaTableMappedObject
  {
  public:
    typedef std::shared_ptr<LuaCountryState> Ptr;
    typedef std::map<int, Ptr> Map;

    explicit LuaCountryState(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaCountryState();

    std::shared_ptr<luaHelper::LuaValueGroupTable> getValuesTable() const;
    std::shared_ptr<LuaBranchesActivatedTable>     getBranchesActivatedTable() const;

    void addNeighbourState(int cid, luabridge::LuaRef& state);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}