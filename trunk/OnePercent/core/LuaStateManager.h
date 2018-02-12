#pragma once

#include <osgGaming/Injector.h>

#include <memory>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class LuaClassInstance
  {
  public:
    virtual ~LuaClassInstance() {}
    virtual void registerClass(lua_State* state) = 0;
    virtual std::string instanceVariableName() { return ""; };

  };

  class LuaStateManager : public osg::Referenced
  {
  public:
    explicit LuaStateManager(osgGaming::Injector& injector);
    ~LuaStateManager();

    void loadScript(std::string filename);
    void printStack();
    void printTable();

    template<typename T>
    void registerClassInstance(LuaClassInstance* inst)
    {
      T* tinst = dynamic_cast<T*>(inst);
      if (tinst == nullptr)
      {
        assert(false && "T should derive from LuaClassInstance");
        return;
      }

      std::string varName = inst->instanceVariableName();

      inst->registerClass(m_state);

      if (varName.empty())
        return;

      luabridge::push<T*>(m_state, tinst);
      lua_setglobal(m_state, varName.c_str());
      lua_pop(m_state, 1);
    }

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

    lua_State* m_state;
  };
}