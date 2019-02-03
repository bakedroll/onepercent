#include "scripting/LuaBranchesActivatedTable.h"

#include "core/Multithreading.h"

#include <QMutex>

namespace onep
{
  struct LuaBranchesActivatedTable::Impl
  {
    Impl() {}

    BranchActivatedMap oActivatedBranches;
    QMutex mutexActivated;
  };

  LuaBranchesActivatedTable::LuaBranchesActivatedTable(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    assert_return(object.isTable());
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
    luaref()[name] = activated;
    Multithreading::uiExecuteOrAsync([=]()
    {
      QMutexLocker lock(&m->mutexActivated);
      getOBranchActivated(name)->set(activated);
    });
  }

  void LuaBranchesActivatedTable::updateObservables()
  {
    foreachElementDo([this](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(value.type() == LUA_TBOOLEAN);
      assert_return(key.isString());

      bool activated = bool(value);

      QMutexLocker lock(&m->mutexActivated);

      osgGaming::Observable<bool>::Ptr oActivated = m->oActivatedBranches[key.tostring()];
      assert_return(oActivated.valid());

      if (oActivated->get() != activated)
        oActivated->set(activated);
    });
  }

  osgGaming::Observable<bool>::Ptr LuaBranchesActivatedTable::getOBranchActivated(const std::string& name) const
  {
    assert_return(m->oActivatedBranches.count(name) > 0, osgGaming::Observable<bool>::Ptr());
    return m->oActivatedBranches.find(name)->second;
  }
}