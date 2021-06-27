#pragma once

#include "scripting/LuaTableMappedObject.h"
#include "scripting/LuaArrayTable.h"
#include "scripting/LuaValueDef.h"

namespace onep
{
  class LuaCountriesTable;
  class LuaBranchesTable;
  class LuaSimulationStateTable;

  class LuaModel : public LuaTableMappedObject
  {
  public:
    using Ptr = std::shared_ptr<LuaModel>;

    LuaModel(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaModel();

    std::shared_ptr<LuaCountriesTable>          getCountriesTable() const;
    std::shared_ptr<LuaBranchesTable>           getBranchesTable() const;
    std::shared_ptr<LuaSimulationStateTable>    getSimulationStateTable() const;
    std::shared_ptr<LuaArrayTable<LuaValueDef>> getValuesDefTable() const;

private:
    struct Impl;
    std::shared_ptr<Impl> m;

  };
}