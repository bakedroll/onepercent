#include "LuaStateManager.h"

#include "core/Macros.h"

#include <osgGaming/ResourceManager.h>
#include <algorithm>

#include <QMutex>
#include <QString>
#include <QStringList>

namespace onep
{
  struct LuaStateManager::Impl
  {
    Impl(osgGaming::Injector& injector)
      : resourceManager(injector.inject<osgGaming::ResourceManager>())
    {}

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    QMutex luaLock;
  };

  LuaStateManager::LuaStateManager(osgGaming::Injector& injector)
    : m(new Impl(injector))
  {
    QMutexLocker lock(&m->luaLock);

    m_state = luaL_newstate();

    luaL_openlibs(m_state);
  }

  LuaStateManager::~LuaStateManager()
  {
    QMutexLocker lock(&m->luaLock);
    lua_close(m_state);
  }

  luabridge::LuaRef LuaStateManager::getGlobal(const char* name) const
  {
    QMutexLocker lock(&m->luaLock);
    return luabridge::getGlobal(m_state, name);
  }

  luabridge::LuaRef LuaStateManager::getObject(const char* name) const
  {
    QMutexLocker lock(&m->luaLock);
    QString namestr(name);

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

    return current;
  }

  bool LuaStateManager::executeCode(std::string code)
  {
    QMutexLocker lock(&m->luaLock);

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
    std::string script = m->resourceManager->loadText(filename);
    bool success = executeCode(script);
    m->resourceManager->clearCacheResource(filename);

    return success;
  }

  void LuaStateManager::printStack()
  {
    QMutexLocker lock(&m->luaLock);

    int i;
    int top = lua_gettop(m_state);
    std::string str;

    printf("Lua Stack dump:\n");

    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
      int t = lua_type(m_state, i);
      switch (t) {
      case LUA_TSTRING:  /* strings */
        str = lua_tostring(m_state, i);
        std::replace(str.begin(), str.end(), '\r', '\n');

        printf("string: '%s'\n", str.c_str());
        break;
      case LUA_TBOOLEAN:  /* booleans */
        printf("boolean %s\n", lua_toboolean(m_state, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:  /* numbers */
        printf("number: %g\n", lua_tonumber(m_state, i));
        break;
      default:  /* other values */
        printf("%s\n", lua_typename(m_state, t));
        break;
      }
      printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
  }

  void LuaStateManager::printTable()
  {
    QMutexLocker lock(&m->luaLock);

    lua_pushnil(m_state);

    while (lua_next(m_state, -2) != 0)
    {
      if (lua_isstring(m_state, -1))
        printf("%s = %s\n", lua_tostring(m_state, -2), lua_tostring(m_state, -1));
      else if (lua_isnumber(m_state, -1))
        printf("%s = %d\n", lua_tostring(m_state, -2), int(lua_tonumber(m_state, -1)));
      else if (lua_istable(m_state, -1))
        printTable();

      lua_pop(m_state, 1);
    }
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