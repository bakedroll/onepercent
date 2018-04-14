#pragma once

#include "simulation/SkillsContainer.h"

#include <osgGaming/Observable.h>

namespace onep
{
  class CountryState : public osg::Referenced, public LuaObjectMapper
  {
  public:
    typedef osg::ref_ptr<CountryState> Ptr;
    typedef std::map<int, Ptr> Map;

    typedef std::map<std::string, float> ValuesMap;
    typedef std::map<std::string, ValuesMap> BranchValuesMap;

    CountryState(const luabridge::LuaRef& object);
    ~CountryState();

    ValuesMap& getValuesMap() const;
    BranchValuesMap& getBranchValuesMap() const;

    bool getBranchActivated(const char* branchName) const;
    void setBranchActivated(const char* branchName, bool activated);
    osgGaming::Observable<bool>::Ptr getOActivatedBranch(const char* branchName) const;

    void writeValues();
    void writeBranchValues();
    void writeBranchesActivated();

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override;
    virtual void readObject(const luabridge::LuaRef& object) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}