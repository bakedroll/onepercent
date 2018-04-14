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
        
        branches.push_back(new SkillBranch(ref, int(branches.size())));
      }
    }

    ~LuaBranchesTable() {}

    SkillBranch::List branches;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override {}
    virtual void readObject(const luabridge::LuaRef& object) override {}

  };


  struct SkillsContainer::Impl
  {
    Impl() {}

    std::shared_ptr<LuaBranchesTable> branchesTable;
    std::map<std::string, int> nameIndexMap;
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
    return m->branchesTable->branches[m->nameIndexMap[name]];
  }

  void SkillsContainer::loadFromLua(const luabridge::LuaRef branches)
  {
    m->branchesTable.reset(new LuaBranchesTable(branches));
  }

}