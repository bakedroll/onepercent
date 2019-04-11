#include "LuaConfig.h"

#include "scripting/LuaStateManager.h"

#include <unordered_map>

#define CONFIG_DATA_TABLE_NAME "config_data"

namespace onep
{
  void LuaConfig::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .beginClass<LuaConfig>("Config")
      .addFunction("extend", &LuaConfig::luaExtend)
      .endClass();
  }

  struct LuaConfig::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {
      lua->createGlobalTable(CONFIG_DATA_TABLE_NAME);
    }

    typedef std::unordered_map<std::string, std::shared_ptr<ValueTypeBase>> Cache;

    osg::ref_ptr<LuaStateManager> lua;
    Cache cache;

  };

  LuaConfig::LuaConfig(osgGaming::Injector& injector)
    : m(new Impl(injector))
  {
  }

  LuaConfig::~LuaConfig()
  {
    clearCache();
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

    auto configDataTable = m->lua->getGlobal(CONFIG_DATA_TABLE_NAME);
    assert_return(m->lua->checkIsType(configDataTable, LUA_TTABLE));

    mergeFunc(configDataTable, ref);
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
      std::string path = CONFIG_DATA_TABLE_NAME + std::string(".") + name;

      luabridge::LuaRef ref = m->lua->getObject(path.c_str());
      result = getFunc(ref);
    });

    m->cache[name] = result;
    return result;
  }
}