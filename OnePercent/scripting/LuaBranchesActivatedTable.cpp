#include "scripting/LuaBranchesActivatedTable.h"


#include "osgHelper/Macros.h"
#include "QtOsgBridge/Multithreading.h"

#include <QMutex>

namespace onep
{
  struct LuaBranchesActivatedTable::Impl
  {
    Impl() = default;

    BranchActivatedMap oActivatedBranches;
    QMutex             mutexActivated;
  };

  LuaBranchesActivatedTable::LuaBranchesActivatedTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
  }

  LuaBranchesActivatedTable::~LuaBranchesActivatedTable() = default;

  void LuaBranchesActivatedTable::addBranchActivated(const std::string& name)
  {
    auto init                   = false;
    luaRef()[name]              = init;
    m->oActivatedBranches[name] = new osgHelper::Observable<bool>(init);
  }

  bool LuaBranchesActivatedTable::getBranchActivated(const std::string& name) const
  {
    QMutexLocker lock(&m->mutexActivated);
    return getOBranchActivated(name)->get();
  }

  void LuaBranchesActivatedTable::setBranchActivated(const std::string& name, bool activated)
  {
    QMutexLocker lock(&m->mutexActivated);
    luaRef()[name] = activated;
    getOBranchActivated(name)->set(activated);
  }

  void LuaBranchesActivatedTable::updateObservables()
  {
    QMutexLocker lock(&m->mutexActivated);

    auto table = luaRef();
    for (const auto& oActivated : m->oActivatedBranches)
    {
      auto activated = static_cast<bool>(table[oActivated.first]);

      if (oActivated.second->get() != activated)
      {
        oActivated.second->set(activated);
      }
    }
  }

  osgHelper::Observable<bool>::Ptr LuaBranchesActivatedTable::getOBranchActivated(const std::string& name) const
  {
    assert_return(m->oActivatedBranches.count(name) > 0, osgHelper::Observable<bool>::Ptr());
    return m->oActivatedBranches.find(name)->second;
  }

  const LuaBranchesActivatedTable::BranchActivatedMap& LuaBranchesActivatedTable::getBranchActivatedMap() const
  {
    return m->oActivatedBranches;
  }

}
