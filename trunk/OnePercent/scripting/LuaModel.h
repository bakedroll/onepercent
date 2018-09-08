#pragma once

#include "scripting/LuaObjectMapper.h"

namespace onep
{
  class LuaCountriesTable;
  class LuaBranchesTable;
  class LuaSimulationStateTable;

  class LuaModel : public LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaModel>;

    LuaModel(const luabridge::LuaRef& object);
    ~LuaModel();

    LuaCountriesTable* getCountriesTable() const;
    LuaBranchesTable* getBranchesTable() const;
    LuaSimulationStateTable* getSimulationStateTable() const;

  private:
    struct Impl;
    std::shared_ptr<Impl> m;
  };
}