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

    LuaModel(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaModel();

    std::shared_ptr<LuaCountriesTable> getCountriesTable() const;
    std::shared_ptr<LuaBranchesTable> getBranchesTable() const;
    std::shared_ptr<LuaSimulationStateTable> getSimulationStateTable() const;

  protected:
    void onTraverse(int type, luabridge::LuaRef& ref) override;

  private:
    struct Impl;
    std::shared_ptr<Impl> m;

  };
}