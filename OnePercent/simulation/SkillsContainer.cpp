#include "simulation/SkillsContainer.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaObjectMapper.h"

#include <LuaBridge.h>

namespace onep
{
  struct SkillsContainer::Impl
  {
    Impl() {}

    std::shared_ptr<LuaBranchesTable> branchesTable;
  };


  SkillsContainer::SkillsContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  SkillsContainer::~SkillsContainer()
  {
  }

  int SkillsContainer::getNumBranches()
  {
    return m->branchesTable->getNumElements();
  }

  LuaSkillBranch::Ptr SkillsContainer::getBranchByIndex(int i)
  {
    return m->branchesTable->getBranchByIndex(i);
  }

  LuaSkillBranch::Ptr SkillsContainer::getBranchByName(std::string name)
  {
    return m->branchesTable->getMappedElement<LuaSkillBranch>(name);
  }

  void SkillsContainer::loadFromLua(const luabridge::LuaRef branches)
  {
    m->branchesTable.reset(new LuaBranchesTable(branches));
  }

}