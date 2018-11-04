#include "LuaStateManager.h"

#include "core/Macros.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/Macros.h>

#include <algorithm>
#include <sstream>

#include <QString>
#include <QStringList>
#include <QFile>

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
}

namespace onep
{
  struct LuaStateManager::Impl
  {
    Impl(osgGaming::Injector& injector)
      : resourceManager(injector.inject<osgGaming::ResourceManager>())
    {}

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;

    luabridge::LuaRef& getLuaFunction(std::string name, LuaRefPtr& cache)
    {
      if (!cache)
      {
        
      }

      return *cache.get();
    }

    std::map<QString, LuaRefPtr> objectCache;
  };

  LuaStateManager::LuaStateManager(osgGaming::Injector& injector)
    : m(new Impl(injector))
  {
    QMutexLocker lock(&m_luaLock);

    m_state = luaL_newstate();

    luaL_openlibs(m_state);
  }

  LuaStateManager::~LuaStateManager()
  {
    QMutexLocker lock(&m_luaLock);
    lua_close(m_state);
  }

  luabridge::LuaRef LuaStateManager::getGlobal(const char* name) const
  {
    return luabridge::getGlobal(m_state, name);
  }

  luabridge::LuaRef LuaStateManager::getObject(const char* name) const
  {
    QString namestr(name);
    if (m->objectCache.count(namestr) > 0)
    {
      return *m->objectCache[namestr].get();
    }

    QStringList names = namestr.split('.');

    assert_return(names.size() > 0, luabridge::LuaRef(m_state));

    std::string currentName = names[0].toStdString();
    luabridge::LuaRef first = luabridge::getGlobal(m_state, currentName.c_str());
    luabridge::LuaRef current = first;

    for (int i = 1; i < names.size(); i++)
    {
      assert_return(current.isTable(), luabridge::LuaRef(m_state));

      currentName = names[i].toStdString();
      luabridge::LuaRef next = current[currentName.c_str()];
      current = next;
    }

    m->objectCache[namestr] = MAKE_LUAREF_PTR(current);
    return current;
  }

  luabridge::LuaRef LuaStateManager::newTable() const
  {
    return luabridge::newTable(m_state);
  }

  luabridge::LuaRef LuaStateManager::createGlobalTable(const std::string& name) const
  {
    luabridge::LuaRef table = newTable();
    luabridge::setGlobal(m_state, table, name.c_str());
    return table;
  }

  luabridge::LuaRef LuaStateManager::createTable(const std::string& name, const luabridge::LuaRef& parentTable)
  {
    luabridge::LuaRef table = newTable();
    parentTable[name] = table;
    return table;
  }

  luabridge::LuaRef LuaStateManager::copyTable(const luabridge::LuaRef& table) const
  {
    auto copyFunc = getGlobal("helper.deepcopy");
    assert_return(copyFunc.isFunction(), luabridge::LuaRef(m_state));

    return copyFunc(table);
  }

  void LuaStateManager::setGlobal(const char* name, const luabridge::LuaRef& ref)
  {
    luabridge::setGlobal(m_state, ref, name);
  }

  bool LuaStateManager::executeCode(std::string code)
  {
    QMutexLocker lock(&m_luaLock);

    if (luaL_dostring(m_state, code.c_str()))
    {
      std::string msg = lua_tostring(m_state, -1);

      std::replace(msg.begin(), msg.end(), '\r', '\n');
      OSGG_LOG_FATAL("Lua Error: " + msg);

      lua_pop(m_state, 1);
      return true;
    }

    return false;
  }

  bool LuaStateManager::loadScript(std::string filename)
  {
    std::string script;

    if (filename[0] == ':')
    {
      auto qfilename = QString::fromStdString(filename);
      QFile file(qfilename);
      if (!file.open(QIODevice::ReadOnly))
      {
        OSGG_QLOG_WARN(QString("Could not load resource file %1.").arg(qfilename));
        return false;
      }

      script = file.readAll().toStdString();
      file.close();
    }
    else
    {
      script = m->resourceManager->loadText(filename);
    }

    return executeCode(script);
  }

  std::string LuaStateManager::getStackTrace() const
  {
    std::stringstream ss;
    lua_Debug info;
    int level = 0;
    while (lua_getstack(m_state, level, &info))
    {
      if (level > 0)
        ss << std::endl;

      lua_getinfo(m_state, "nSl", &info);
      QString current = QString("  [%1] %2:%3 -- [%4]")
        .arg(level)
        .arg(info.short_src)
        .arg(info.currentline)
        .arg((info.name ? info.name : "<unknown>"));
      ss << current.toStdString();
      ++level;
    }

    return ss.str();
  }

  bool LuaStateManager::checkIsType(const luabridge::LuaRef& ref, int luaType)
  {
    if (ref.type() == luaType)
    {
      return true;
    }

    std::string message = "Lua typecheck failed\n";
    message.append(getStackTrace());
    OSGG_LOG_WARN(message);

    return false;
  }

  void LuaStateManager::safeExecute(std::function<void()> func)
  {
    try
    {
      func();
    }
    catch (luabridge::LuaException& e)
    {
      OSGG_QLOG_FATAL(QString("Lua Exception: %1").arg(e.what()));
    }
  }

}
