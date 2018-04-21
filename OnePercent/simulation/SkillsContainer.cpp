#include "SkillsContainer.h"

#include "core/Macros.h"
#include "scripting/LuaObjectMapper.h"

#include <LuaBridge.h>

namespace onep
{
  class LuaBranchesTable : public LuaObjectMapper
  {
  public:
    LuaBranchesTable(const luabridge::LuaRef& object)
      : LuaObjectMapper(object)
    {
      assert_return(object.isTable());

      for (luabridge::Iterator it(object); !it.isNil(); ++it)
      {
        luabridge::LuaRef ref = *it;
        assert_continue(ref.isTable());
        
        SkillBranch::Ptr branch = new SkillBranch(ref, int(branches.size()));
        branches.push_back(branch);
        nameIndexMap[branch->getBranchName()] = int(branches.size()) - 1;
      }
    }

    ~LuaBranchesTable() {}

    SkillBranch::List branches;
    std::map<std::string, int> nameIndexMap;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override {}
    virtual void readObject(const luabridge::LuaRef& object) override {}

  };


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
    return int(m->branchesTable->branches.size());
  }

  SkillBranch::Ptr SkillsContainer::getBranchByIndex(int i)
  {
    return m->branchesTable->branches[i];
  }

  SkillBranch::Ptr SkillsContainer::getBranchByName(std::string name)
  {
    return m->branchesTable->branches[m->branchesTable->nameIndexMap[name]];
  }

  void SkillsContainer::loadFromLua(const luabridge::LuaRef branches)
  {
    m->branchesTable.reset(new LuaBranchesTable(branches));
  }

}