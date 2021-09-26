#include "scripting/LuaCountryState.h"

#include <luaHelper/LuaTableMappedObject.h>

namespace onep
{
  struct LuaCountryState::Impl
  {
    Impl() = default;

    std::shared_ptr<luaHelper::LuaValueGroupTable> tValues;
    std::shared_ptr<LuaBranchesActivatedTable>     tBranchesActivated;
  };

  LuaCountryState::LuaCountryState(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
    , m(new Impl())
  {
    m->tValues            = newMappedObject<luaHelper::LuaValueGroupTable>("values");
    m->tBranchesActivated = newMappedObject<LuaBranchesActivatedTable>("branches_activated");

    newMappedObject<LuaTableMappedObject>("neighbour_states");
  }

  LuaCountryState::~LuaCountryState() = default;

  std::shared_ptr<luaHelper::LuaValueGroupTable> LuaCountryState::getValuesTable() const
  {
    return m->tValues;
  }

  std::shared_ptr<LuaBranchesActivatedTable> LuaCountryState::getBranchesActivatedTable() const
  {
    return m->tBranchesActivated;
  }

  void LuaCountryState::addNeighbourState(int cid, luabridge::LuaRef& state)
  {
    auto table = getMappedObject<LuaTableMappedObject>("neighbour_states");
    table->setValue(std::to_string(cid), state);
  }
}
