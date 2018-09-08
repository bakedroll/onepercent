#pragma once

#include "scripting/LuaObjectMapper.h"

#include <osgGaming/Observable.h>

namespace onep
{
  class SkillBranch;

  class LuaBranchesActivatedTable : public LuaObjectMapper
  {
  public:
    typedef std::map<std::string, osgGaming::Observable<bool>::Ptr> BranchActivatedMap;

    explicit LuaBranchesActivatedTable(const luabridge::LuaRef& object);
    ~LuaBranchesActivatedTable();

    bool getBranchActivated(const std::string& name) const;
    void setBranchActivated(const std::string& name, bool activated);

    osgGaming::Observable<bool>::Ptr getOBranchActivated(const std::string& name) const;

  protected:
    virtual void onUpdate(luabridge::LuaRef& object) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}