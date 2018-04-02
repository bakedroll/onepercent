#pragma once

#include "core/LuaStateManager.h"

#include <osgGaming/Injector.h>

#include <osg/Referenced>
#include <memory>

namespace onep
{
  class SkillBranch;

  class SkillsContainer : public osg::Referenced, public LuaClassInstance
  {
  public:
    typedef osg::ref_ptr<SkillsContainer> Ptr;

    SkillsContainer(osgGaming::Injector& injector);
    ~SkillsContainer();

    void addSkillBranch(osg::ref_ptr<SkillBranch> branch);

    int getNumBranches();
    osg::ref_ptr<SkillBranch> getBranchByIndex(int i);
    osg::ref_ptr<SkillBranch> getBranchByName(std::string name);

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    // Lua functions
    void lua_add_branches(lua_State* state);
    void lua_add_skills(lua_State* state);
    
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}