#include "LuaConfig.h"

#include "scripting/LuaStateManager.h"
#include "scripting/LuaMapTable.h"

#include <unordered_map>

#define CONFIG_DATA_PATH "config_data"

namespace onep
{
  struct LuaConfig::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {
      configDataTable = lua->createElement<LuaMapTable>(CONFIG_DATA_PATH);
    }

    typedef std::unordered_map<std::string, std::shared_ptr<ValueTypeBase>> Cache;

    osg::ref_ptr<LuaStateManager> lua;

    LuaMapTable::Ptr configDataTable;
    Cache cache;

  };

  LuaConfig::LuaConfig(osgGaming::Injector& injector)
    : m(new Impl(injector))
    , LuaClassInstance("config")
  {
  }

  LuaConfig::~LuaConfig()
  {
    clearCache();
  }

  void LuaConfig::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaConfig>("Config")
      .addFunction("extend", &LuaConfig::luaExtend)
      .endClass();
  }

  void LuaConfig::clearCache()
  {
    m->cache.clear();
  }

  void LuaConfig::luaExtend(luabridge::LuaRef ref)
  {
    assert_return(m->lua->checkIsType(ref, LUA_TTABLE));

    auto mergeFunc = m->lua->getObject("helper.merge");
    assert_return(m->lua->checkIsType(mergeFunc, LUA_TFUNCTION));

    mergeFunc(m->configDataTable->luaref(), ref);
  }

  std::shared_ptr<LuaConfig::ValueTypeBase> LuaConfig::getValuePtr(
    const std::string& name,
    std::function<std::shared_ptr<ValueTypeBase>(luabridge::LuaRef& ref)> getFunc)
  {
    Impl::Cache::iterator it = m->cache.find(name);
    if (it != m->cache.end())
      return it->second;

    std::shared_ptr<ValueTypeBase> result;
    m->lua->safeExecute([&]()
    {
      std::string path = CONFIG_DATA_PATH + std::string(".") + name;

      luabridge::LuaRef ref = m->lua->getObject(path.c_str());
      result = getFunc(ref);
    });

    m->cache[name] = result;
    return result;
  }
}