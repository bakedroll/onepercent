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

    explicit LuaBranchesActivatedTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaBranchesActivatedTable();

    void addBranchActivated(const std::string& name);

    bool getBranchActivated(const std::string& name) const;
    void setBranchActivated(const std::string& name, bool activated);

    osgGaming::Observable<bool>::Ptr getOBranchActivated(const std::string& name) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}