#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaValuesDefTable.h"
#include "core/Enums.h"

namespace onep
{
  struct LuaModel::Impl
  {
    Impl() {}
    ~Impl() {}

    LuaCountriesTable::Ptr countriesTable;
    LuaBranchesTable::Ptr branchesTable;
    LuaSimulationStateTable::Ptr stateTable;
    LuaValuesDefTable::Ptr valuesTable;
  };

  LuaModel::LuaModel(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
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

  void LuaModel::onTraverse(int type, luabridge::LuaRef& ref)
  {
    if (type != static_cast<int>(ModelTraversalType::BOOTSTRAP))
      return;

    m->countriesTable = newMappedElement<LuaCountriesTable>("countries");
    m->branchesTable = newMappedElement<LuaBranchesTable>("branches");
    m->stateTable = newMappedElement<LuaSimulationStateTable>("state");
    m->valuesTable = newMappedElement<LuaValuesDefTable>("values");
  }
}