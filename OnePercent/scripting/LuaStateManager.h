#pragma once

#include "scripting/LuaClassDefinition.h"

#include <osgGaming/Injector.h>

#include <memory>
#include <functional>

#include <QMutex>

extern "C"
{
#include <lua.h>
}

#include <LuaBridge/LuaBridge.h>

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
    luabridge::LuaRef copyTable(const luabridge::LuaRef& table) const;

    void setGlobal(const char* name, const luabridge::LuaRef& ref);

    bool executeCode(const std::string& code);
    bool loadScript(const std::string& filename);

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

    void safeExecute(std::function<void()> func);

    template<typename LuaClassType>
    void makeGlobalInstance(const std::string name, LuaClassType* inst)
    {
      luabridge::push<LuaClassType*>(m_state, inst);
      lua_setglobal(m_state, name.c_str());
    }

    template<
      typename ClassDefinitionType,
      typename = typename std::enable_if<std::is_base_of<LuaClassDefinition, ClassDefinitionType>::value>::type>
      void registerClass()
      {
        ClassDefinitionType().registerClass(m_state);
      }

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

    mutable QMutex m_luaLock;
    lua_State* m_state;

  };
}