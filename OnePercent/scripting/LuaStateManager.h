#pragma once

#include "scripting/LuaBridgeDefinition.h"

#include <osgHelper/Macros.h>
#include <osgHelper/ioc/Injector.h>

#include <functional>
#include <memory>

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

  enum class LoadingScriptMode
  {
    LoadDirectlyIfPossible,
    LoadFromResourceOrPackage
  };

  explicit LuaStateManager(osgHelper::ioc::Injector& injector);
  ~LuaStateManager();

  luabridge::LuaRef getGlobal(const char* name) const;
  luabridge::LuaRef getObject(const char* name) const;
  luabridge::LuaRef newTable() const;

  void setGlobal(const char* name, const luabridge::LuaRef& ref);

  bool executeCode(const std::string& code);
  bool loadScript(const std::string& filename, LoadingScriptMode mode = LoadingScriptMode::LoadDirectlyIfPossible);

  std::string getStackTrace() const;
  bool        checkIsType(const luabridge::LuaRef& ref, int luaType);

  template <typename LuaObject>
  std::shared_ptr<LuaObject> createTableMappedObject(const luabridge::LuaRef& table)
  {
    assert_return(table.isTable(), nullptr);
    return std::make_shared<LuaObject>(table, m_state);
  }

  void safeExecute(std::function<void()> func);

  template <typename DefinitionType,
            typename = typename std::enable_if<std::is_base_of<LuaBridgeDefinition, DefinitionType>::value>::type,
            typename... Args>
  void registerDefinition(Args... args)
  {
    DefinitionType(args...).registerDefinition(m_state);
  }

private:
  struct Impl;
  std::unique_ptr<Impl> m;

  mutable QRecursiveMutex m_luaLock;
  lua_State*              m_state;

  bool executeScript(const std::string& filename);
  void logErrorsFromStack() const;

  void logLuaError(const std::string& message) const;

};

}  // namespace onep