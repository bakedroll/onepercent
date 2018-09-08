#pragma once

#include "scripting/LuaClass.h"
#include "scripting/LuaClassInstance.h"

#include <osgGaming/Injector.h>
#include <osgGaming/LogManager.h>

#include <memory>
#include <functional>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
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

    bool executeCode(std::string code);
    bool loadScript(std::string filename);

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

      std::string varName = inst->instanceVariableName();

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

    lua_State* m_state;
  };
}