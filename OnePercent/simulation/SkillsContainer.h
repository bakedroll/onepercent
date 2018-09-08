#pragma once

#include "scripting/LuaSkillBranch.h"

#include <osgGaming/Injector.h>

#include <osg/Referenced>
#include <memory>

namespace onep
{
  class LuaSkillBranch;

  class SkillsContainer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<SkillsContainer> Ptr;

    SkillsContainer(osgGaming::Injector& injector);
    ~SkillsContainer();

    int getNumBranches();
    std::shared_ptr<LuaSkillBranch> getBranchByIndex(int i);
    std::shared_ptr<LuaSkillBranch> getBranchByName(std::string name);

    void loadFromLua(const luabridge::LuaRef branches);
    
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}