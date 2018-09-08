#include "simulation/CountryState.h"
#include "core/Multithreading.h"

namespace onep
{
  struct CountryState::Impl
  {
    Impl() {}

    std::shared_ptr<LuaValuesTable> tValues;
    std::shared_ptr<LuaBranchValuesTable> tBranchValues;
    std::shared_ptr<LuaBranchesActivatedTable> tBranchesActivated;
  };

  CountryState::CountryState(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    m->tValues            = makeMappedElement<LuaValuesTable>("values");
    m->tBranchValues      = makeMappedElement<LuaBranchValuesTable>("branch_values");
    m->tBranchesActivated = makeMappedElement<LuaBranchesActivatedTable>("branches_activated");
  }

  CountryState::~CountryState() = default;

  LuaValuesTable::ValuesMap CountryState::getValuesMap() const
  {
    return m->tValues->getValuesMap();
  }

  LuaBranchValuesTable::BranchValuesMap CountryState::getBranchValuesMap() const
  {
    return m->tBranchValues->getBranchValuesMap();
  }

  std::shared_ptr<LuaValuesTable> CountryState::getValuesTable() const
  {
    return m->tValues;
  }

  std::shared_ptr<LuaBranchValuesTable> CountryState::getBranchValuesTable() const
  {
    return m->tBranchValues;
  }

  std::shared_ptr<LuaBranchesActivatedTable> CountryState::getBranchesActivatedTable() const
  {
    return m->tBranchesActivated;
  }
}
