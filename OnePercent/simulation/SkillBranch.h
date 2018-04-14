#pragma once

#include "Skill.h"
#include "scripting/LuaObjectMapper.h"

#include <osg/ref_ptr>

#include <memory>

namespace onep
{
  class SkillBranch : public osg::Referenced, public LuaObjectMapper
  {
  public:
    typedef osg::ref_ptr<SkillBranch> Ptr;
    typedef std::vector<Ptr> List;

    SkillBranch(const luabridge::LuaRef& object, int id);
    ~SkillBranch();

    int getBranchId() const;
    const std::string& getBranchName() const;
    int getNumSkills() const;
    Skill::Ptr getSkill(int i) const;

    int getCost() const;

  protected:

    virtual void writeObject(luabridge::LuaRef& object) const override;
    virtual void readObject(const luabridge::LuaRef& object) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}