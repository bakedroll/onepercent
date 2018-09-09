#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaCountriesTable.h"

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

  std::shared_ptr<LuaCountriesTable> LuaModel::getCountriesTable() const
  {
    return m->countriesTable;
  }

  std::shared_ptr<LuaBranchesTable> LuaModel::getBranchesTable() const
  {
    return m->branchesTable;
  }

  std::shared_ptr<LuaSimulationStateTable> LuaModel::getSimulationStateTable() const
  {
    return m->stateTable;
  }
}