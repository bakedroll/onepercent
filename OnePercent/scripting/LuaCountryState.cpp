#include "scripting/LuaCountryState.h"
#include "scripting/LuaTableMappedObject.h"

#include "QtOsgBridge/Multithreading.h"

namespace onep
{
  struct LuaCountryState::Impl
  {
    Impl() = default;

    std::shared_ptr<LuaValueGroupTable>        tValues;
    std::shared_ptr<LuaBranchesActivatedTable> tBranchesActivated;
  };

  LuaCountryState::LuaCountryState(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTableMappedObject(object, luaState)
    , m(new Impl())
  {
    m->tValues            = newMappedElement<LuaValueGroupTable>("values");
    m->tBranchesActivated = newMappedElement<LuaBranchesActivatedTable>("branches_activated");

    newMappedElement<LuaTableMappedObject>("neighbour_states");
  }

  LuaCountryState::~LuaCountryState() = default;

  std::shared_ptr<LuaValueGroupTable> LuaCountryState::getValuesTable() const
  {
    return m->tValues;
  }

  std::shared_ptr<LuaBranchesActivatedTable> LuaCountryState::getBranchesActivatedTable() const
  {
    return m->tBranchesActivated;
  }

  void LuaCountryState::addNeighbourState(int cid, luabridge::LuaRef& state)
  {
    auto table = getMappedElement<LuaTableMappedObject>("neighbour_states");
    table->setValue(std::to_string(cid), state);
  }
}
