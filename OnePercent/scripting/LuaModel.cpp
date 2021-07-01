#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaArrayTable.h"

namespace onep
{
  struct LuaModel::Impl
  {
    Impl()  = default;
    ~Impl() = default;

    LuaCountriesTable::Ptr          countriesTable;
    LuaBranchesTable::Ptr           branchesTable;
    LuaSimulationStateTable::Ptr    stateTable;
    LuaArrayTable<LuaValueDef>::Ptr valuesTable;
  };

  LuaModel::LuaModel(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
    , m(new Impl())
  {
    m->countriesTable = newMappedObject<LuaCountriesTable>("countries");
    m->branchesTable  = newMappedObject<LuaBranchesTable>("branches");
    m->stateTable     = newMappedObject<LuaSimulationStateTable>("state");
    m->valuesTable    = newMappedObject<LuaArrayTable<LuaValueDef>>("values");
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

  std::shared_ptr<LuaArrayTable<LuaValueDef>> LuaModel::getValuesDefTable() const
  {
    return m->valuesTable;
  }
}
