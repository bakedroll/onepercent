#pragma once

#include "simulation/SkillBranch.h"
#include "scripting/LuaStateManager.h"

#include <osgGaming/Injector.h>

#include <osg/Referenced>
#include <memory>

namespace onep
{
  class SkillBranch;

  class SkillsContainer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<SkillsContainer> Ptr;

    SkillsContainer(osgGaming::Injector& injector);
    ~SkillsContainer();

    int getNumBranches();
    osg::ref_ptr<SkillBranch> getBranchByIndex(int i);
    osg::ref_ptr<SkillBranch> getBranchByName(std::string name);

    void loadFromLua(const luabridge::LuaRef branches);
    
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}