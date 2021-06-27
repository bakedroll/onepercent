#pragma once

#include "scripting/LuaCountryState.h"

namespace onep
{
  class LuaSimulationStateTable : public LuaTableMappedObject
  {
  public:
    struct CountryBranch
    {
      CountryBranch(int cid, const std::string& branch)
        : countryId(cid)
        , branchName(branch)
      {}

      CountryBranch()
        : CountryBranch(0, "")
      {}

      int         countryId;
      std::string branchName;
    };

    using Ptr                    = std::shared_ptr<LuaSimulationStateTable>;
    using OCountryBranchAcivated = osgHelper::Observable<CountryBranch>;

    explicit LuaSimulationStateTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaSimulationStateTable();

    LuaCountryState::Map& getCountryStates() const;
    LuaCountryState::Ptr getCountryState(int cid) const;

    OCountryBranchAcivated::Ptr getOCountryBranchActivated() const;

    void addCountryState(int id);

    void initializeCountryBranchActivated();
    void updateObservables();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}