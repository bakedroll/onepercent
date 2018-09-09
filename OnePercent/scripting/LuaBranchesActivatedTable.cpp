#include "scripting/LuaBranchesActivatedTable.h"

#include "core/Multithreading.h"
#include "core/Enums.h"

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

    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(value.type() == LUA_TBOOLEAN);
      assert_return(key.isString());

      m->oActivatedBranches[key.tostring()] = new osgGaming::Observable<bool>(bool(value));
    });
  }

  LuaBranchesActivatedTable::~LuaBranchesActivatedTable() = default;

  bool LuaBranchesActivatedTable::getBranchActivated(const std::string& name) const
  {
    QMutexLocker lock(&m->mutexActivated);
    return getOBranchActivated(name)->get();
  }

  void LuaBranchesActivatedTable::setBranchActivated(const std::string& name, bool activated)
  {
    QMutexLocker lock(&m->mutexActivated);
    luaref()[name] = activated;
    Multithreading::uiExecuteOrAsync([=]() { getOBranchActivated(name)->set(activated); });
  }

  osgGaming::Observable<bool>::Ptr LuaBranchesActivatedTable::getOBranchActivated(const std::string& name) const
  {
    assert_return(m->oActivatedBranches.count(name) > 0, osgGaming::Observable<bool>::Ptr());
    return m->oActivatedBranches.find(name)->second;
  }

  void LuaBranchesActivatedTable::onTraverse(int type, luabridge::LuaRef& object)
  {
    if (type != static_cast<int>(ModelTraversalType::TRIGGER_OBSERVABLES))
      return;
    
    foreachElementDo([&](luabridge::LuaRef& key, luabridge::LuaRef& value)
    {
      assert_return(value.type() == LUA_TBOOLEAN);
      assert_return(key.isString());

      bool activated = bool(value);

      osgGaming::Observable<bool>::Ptr oActivated = m->oActivatedBranches[key.tostring()];
      assert_return(oActivated.valid());

      if (oActivated->get() != activated)
        oActivated->set(activated);
    });
  }
}