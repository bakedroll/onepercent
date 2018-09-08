#include "scripting/LuaModel.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSimulationStateTable.h"

namespace onep
{
  struct LuaModel::Impl
  {
    Impl() {}
    ~Impl() {}

    LuaCountriesTable::Ptr countriesTable;
    LuaBranchesTable::Ptr branchesTable;
    LuaSimulationStateTable::Ptr stateTable;
  };

  LuaModel::LuaModel(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    m->countriesTable = makeMappedElement<LuaCountriesTable>("countries");
    m->branchesTable  = makeMappedElement<LuaBranchesTable>("branches");
    m->stateTable     = makeMappedElement<LuaSimulationStateTable>("state");
  }

  LuaModel::~LuaModel() = default;

  LuaCountriesTable* LuaModel::getCountriesTable() const
  {
    auto& a = m->countriesTable;
    auto b = a.get();

    return m->countriesTable.get();
  }

  LuaBranchesTable* LuaModel::getBranchesTable() const
  {
    return m->branchesTable.get();
  }

  LuaSimulationStateTable* LuaModel::getSimulationStateTable() const
  {
    return m->stateTable.get();
  }
}