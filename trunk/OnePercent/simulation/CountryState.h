#pragma once

#include "simulation/SkillsContainer.h"
#include "scripting/LuaStateManager.h"
#include "simulation/SimulatedLuaValue.h"

#include <QString>
#include <osgGaming/Observable.h>

namespace onep
{
  class CountryState : public osg::Referenced, public LuaClass
  {
  public:
    typedef osg::ref_ptr<CountryState> Ptr;
    typedef std::map<int, Ptr> Map;

    typedef std::map<QString, SimulatedLuaValue::Ptr> ValuesMap;
    typedef std::map<QString, ValuesMap> BranchValuesMap;

    CountryState();
    ~CountryState();

    Ptr copy() const;
    void overwrite(Ptr other);

    void addValue(const char* name, float init);
    void addBranchValue(const char* name, SkillsContainer::Ptr skillsContainer, float init);

    ValuesMap& getValuesMap() const;
    BranchValuesMap& getBranchValuesMap() const;

    bool getBranchActivated(const char* branchName) const;
    void setBranchActivated(const char* branchName, bool activated);
    osgGaming::Observable<bool>::Ptr getOActivatedBranch(const char* branchName) const;

    virtual void registerClass(lua_State* state) override;

    // lua
    SimulatedLuaValue* lua_get_value(const char* name);
    SimulatedLuaValue* lua_get_branch_value(const char* name);
    void lua_set_current_branch(std::string branchName);
    bool lua_get_branch_activated() const;
    void lua_set_branch_activated(bool activated);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}