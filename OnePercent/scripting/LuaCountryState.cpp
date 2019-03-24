#include "scripting/LuaCountryState.h"
#include "scripting/LuaMapTable.h"
#include "core/Multithreading.h"

namespace onep
{
  struct LuaCountryState::Impl
  {
    Impl() = default;

    std::shared_ptr<LuaValuesTable> tValues;
    std::shared_ptr<LuaBranchValuesTable> tBranchValues;
    std::shared_ptr<LuaBranchesActivatedTable> tBranchesActivated;
  };

  LuaCountryState::LuaCountryState(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());

    m->tValues            = newMappedElement<LuaValuesTable>("values");
    m->tBranchValues      = newMappedElement<LuaBranchValuesTable>("branch_values");
    m->tBranchesActivated = newMappedElement<LuaBranchesActivatedTable>("branches_activated");

    newMappedElement<LuaMapTable>("neighbour_states");
  }

  LuaCountryState::~LuaCountryState() = default;

  LuaValuesTable::ValuesMap LuaCountryState::getValuesMap() const
  {
    return m->tValues->getValuesMap();
  }

  LuaBranchValuesTable::BranchValuesMap LuaCountryState::getBranchValuesMap() const
  {
    return m->tBranchValues->getBranchValuesMap();
  }

  std::shared_ptr<LuaValuesTable> LuaCountryState::getValuesTable() const
  {
    return m->tValues;
  }

  std::shared_ptr<LuaBranchValuesTable> LuaCountryState::getBranchValuesTable() const
  {
    return m->tBranchValues;
  }

  std::shared_ptr<LuaBranchesActivatedTable> LuaCountryState::getBranchesActivatedTable() const
  {
    return m->tBranchesActivated;
  }

  void LuaCountryState::addNeighbourState(int cid, luabridge::LuaRef& state)
  {
    auto table = getMappedElement<LuaMapTable>("neighbour_states");
    table->insert(std::to_string(cid), state);
  }
}
