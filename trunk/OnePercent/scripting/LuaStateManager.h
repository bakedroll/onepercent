#pragma once

#include "scripting/LuaClass.h"
#include "scripting/LuaClassInstance.h"

#include <osgGaming/Injector.h>
#include <osgGaming/LogManager.h>

#include <memory>
#include <functional>

#include <QMutex>

extern "C"
{
#include <lua.h>
}

#include <LuaBridge.h>

#define MAKE_LUAREF_PTR(luaRef) std::make_shared<luabridge::LuaRef>(luaRef)

namespace onep
{
  typedef std::shared_ptr<luabridge::LuaRef> LuaRefPtr;

  class LuaStateManager : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<LuaStateManager>;

    explicit LuaStateManager(osgGaming::Injector& injector);
    ~LuaStateManager();

    luabridge::LuaRef getGlobal(const char* name) const;
    luabridge::LuaRef getObject(const char* name) const;
    luabridge::LuaRef newTable() const;
    luabridge::LuaRef createGlobalTable(const std::string& name) const;
    luabridge::LuaRef createTable(const std::string& name, const luabridge::LuaRef& parentTable);

    void setGlobal(const char* name, const luabridge::LuaRef& ref);

    bool executeCode(std::string code);
    bool loadScript(std::string filename);

    std::string getStackTrace() const;
    bool checkIsType(const luabridge::LuaRef& ref, int luaType);

    template <typename LuaObject>
    std::shared_ptr<LuaObject> makeGlobalElement(const char* key)
    {
      QMutexLocker lock(&m_luaLock);
      luabridge::LuaRef ref = luabridge::getGlobal(m_state, key);
      return std::make_shared<LuaObject>(ref, m_state);
    }

    template <typename LuaObject>
    std::shared_ptr<LuaObject> createElement(const std::string& name)
    {
      luabridge::LuaRef table = createGlobalTable(name);
      return std::make_shared<LuaObject>(table, m_state);
    }

    template <typename LuaObject>
    std::shared_ptr<LuaObject> createElement(const std::string& name, const luabridge::LuaRef& parentTable)
    {
      luabridge::LuaRef table = createTable(name, parentTable);
      return std::make_shared<LuaObject>(table, m_state);
    }

    static void safeExecute(std::function<void()> func);

    template<typename T>
    void registerClassInstance(LuaClassInstance* inst)
    {
      T* tinst = dynamic_cast<T*>(inst);
      if (tinst == nullptr)
      {
        OSGG_LOG_WARN("T should derive from LuaClassInstance");

        assert(false);
        return;
      }

      auto varName = inst->getInstanceVariableName();

      inst->registerClass(m_state);

      luabridge::push<T*>(m_state, tinst);
      lua_setglobal(m_state, varName.c_str());
    }

    template<typename T>
    void registerClass()
    {
      T inst;
      if (dynamic_cast<LuaClass*>(&inst) == nullptr)
      {
        OSGG_LOG_WARN("T should derive from LuaClass");

        assert(false);
        return;
      }

      inst.registerClass(m_state);
    }

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

    mutable QMutex m_luaLock;
    lua_State* m_state;

  };
}