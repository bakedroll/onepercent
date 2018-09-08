#include "LuaConfig.h"

#include "scripting/LuaStateManager.h"

#include <unordered_map>

#define CONFIG_PATH "config"

namespace onep
{
  struct LuaConfig::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {}

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
      std::string path = CONFIG_PATH + std::string(".") + name;

      luabridge::LuaRef ref = m->lua->getObject(path.c_str());
      result = getFunc(ref);
    });

    m->cache[name] = result;
    return result;
  }
}