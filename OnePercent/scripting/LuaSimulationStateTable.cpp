#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaCountryState.h"

#include <osgGaming/Macros.h>

#include <QMutex>

namespace onep
{
  struct LuaSimulationStateTable::Impl
  {
    Impl()
      : oCountryBranchActivated(new OCountryBranchAcivated({}))
    {}

    LuaCountryState::Map        countryStates;
    OCountryBranchAcivated::Ptr oCountryBranchActivated;

    std::vector<osgGaming::Observer<bool>::Ptr> countryBranchObservers;

    QMutex cbActivatedMutex;
  };

  LuaSimulationStateTable::LuaSimulationStateTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
  }

  LuaSimulationStateTable::~LuaSimulationStateTable() = default;

  LuaCountryState::Map& LuaSimulationStateTable::getCountryStates() const
  {
    return m->countryStates;
  }

  LuaCountryState::Ptr LuaSimulationStateTable::getCountryState(int cid) const
  {
    return getMappedElement<LuaCountryState>(cid);
  }

  osgGaming::Observable<LuaSimulationStateTable::CountryBranch>::Ptr LuaSimulationStateTable::
  getOCountryBranchActivated() const
  {
      return m->oCountryBranchActivated;
  }

  void LuaSimulationStateTable::addCountryState(int id)
  {
    m->countryStates[id] = newMappedElement<LuaCountryState>(id);
  }

  void LuaSimulationStateTable::initializeCountryBranchActivated()
  {
    for (const auto& state : m->countryStates)
    {
      auto cid = state.first;
      for (const auto& it : state.second->getBranchesActivatedTable()->getBranchActivatedMap())
      {
        auto branchName = it.first;
        m->countryBranchObservers.push_back(it.second->connect(osgGaming::Func<bool>([this, branchName, cid](bool activated)
        {
          if (!activated)
          {
            return;
          }

          QMutexLocker locker(&m->cbActivatedMutex);
          m->oCountryBranchActivated->set({ cid, branchName });
        })));
      }
    }
  }

  void LuaSimulationStateTable::updateObservables()
  {
    for (const auto& state : m->countryStates)
    {
      state.second->getBranchesActivatedTable()->updateObservables();
    }
  }
}
