#include "scripting/LuaBranchesActivatedTable.h"

#include "core/Multithreading.h"

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
    luaref()[name]              = init;
    m->oActivatedBranches[name] = new osgGaming::Observable<bool>(init);
  }

  bool LuaBranchesActivatedTable::getBranchActivated(const std::string& name) const
  {
    QMutexLocker lock(&m->mutexActivated);
    return getOBranchActivated(name)->get();
  }

  void LuaBranchesActivatedTable::setBranchActivated(const std::string& name, bool activated)
  {
    QMutexLocker lock(&m->mutexActivated);
    luaref()[name] = activated;
    getOBranchActivated(name)->set(activated);
  }

  void LuaBranchesActivatedTable::updateObservables()
  {
    QMutexLocker lock(&m->mutexActivated);

    auto table = luaref();
    for (const auto& oActivated : m->oActivatedBranches)
    {
      auto activated = static_cast<bool>(table[oActivated.first]);

      if (oActivated.second->get() != activated)
      {
        oActivated.second->set(activated);
      }
    }
  }

  osgGaming::Observable<bool>::Ptr LuaBranchesActivatedTable::getOBranchActivated(const std::string& name) const
  {
    assert_return(m->oActivatedBranches.count(name) > 0, osgGaming::Observable<bool>::Ptr());
    return m->oActivatedBranches.find(name)->second;
  }

  const LuaBranchesActivatedTable::BranchActivatedMap& LuaBranchesActivatedTable::getBranchActivatedMap() const
  {
    return m->oActivatedBranches;
  }

}