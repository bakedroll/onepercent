#include "LuaStateManager.h"

#include "core/Macros.h"
#include "scripting/LuaInvalidDataException.h"

#include <osgGaming/ResourceManager.h>
#include <osgGaming/Macros.h>

#include <algorithm>
#include <sstream>

#include <QStringList>
#include <QFile>

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
}

namespace onep
{
  struct LuaStateManager::Impl
  {
    Impl(osgGaming::Injector& injector)
      : resourceManager(injector.inject<osgGaming::ResourceManager>())
    {}

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    std::map<QString, LuaRefPtr>             objectCache;

  };

  LuaStateManager::LuaStateManager(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
    , m_luaLock(QMutex::Recursive)
  {
    QMutexLocker lock(&m_luaLock);

    m_state = luaL_newstate();

    luaL_openlibs(m_state);
  }

  LuaStateManager::~LuaStateManager()
  {
    QMutexLocker lock(&m_luaLock);
    m->objectCache.clear();

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

    assert_return(!names.empty(), luabridge::LuaRef(m_state));

    std::string       currentName = names[0].toStdString();
    luabridge::LuaRef first       = luabridge::getGlobal(m_state, currentName.c_str());
    luabridge::LuaRef current     = first;

    for (auto i = 1; i < names.size(); i++)
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

  bool LuaStateManager::executeCode(const std::string& code)
  {
    auto success = true;
    safeExecute([this, &success, &code]()
    {
      if (luaL_dostring(m_state, code.c_str()) != LUA_OK)
      {
        logErrorsFromStack();
        success = false;
      }
    });

    return success;
  }

  bool LuaStateManager::loadScript(const std::string& filename, LoadingScriptMode mode)
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
    else if (mode == LoadingScriptMode::LoadFromResourceOrPackage)
    {
      script = m->resourceManager->loadText(filename);
      return executeCode(script);
    }

    return executeScript(filename);
  }

  std::string LuaStateManager::getStackTrace() const
  {
    QString result;
    auto    lines = 0;

    luaL_traceback(m_state, m_state, nullptr, 0);
    while (lua_gettop(m_state) > 0)
    {
      result.append(QString("%1\n").arg(lua_tostring(m_state, -1)));
      lua_pop(m_state, 1);
      lines++;
    }

    return (lines > 1 ? result.toStdString() : "");
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
      QMutexLocker locker(&m_luaLock);
      func();
    }
    catch (luabridge::LuaException& e)
    {
      logLuaError(e.what());
    }
    catch (LuaInvalidDataException& e)
    {
      logLuaError(e.what());
    }
  }

  bool LuaStateManager::executeScript(const std::string& filename)
  {
    auto success = true;
    safeExecute([this, &success, &filename]()
    {
      if (luaL_dofile(m_state, filename.c_str()) != LUA_OK)
      {
        logErrorsFromStack();
        success = false;
      }
    });

    return success;
  }

  void LuaStateManager::logErrorsFromStack() const
  {
    std::string msg = lua_tostring(m_state, -1);

    std::replace(msg.begin(), msg.end(), '\r', '\n');
    lua_pop(m_state, 1);

    logLuaError(msg);
  }

  void LuaStateManager::logLuaError(const std::string& message) const
  {
    auto log = QString("Lua Error: %1").arg(message.c_str());

    /*const auto stacktrace = getStackTrace();
    if (!stacktrace.empty() && (stacktrace != "stack traceback:"))
    {
      log.append(QString("\n%1").arg(stacktrace.c_str()));
    }*/

    OSGG_QLOG_FATAL(log);
  }
}
