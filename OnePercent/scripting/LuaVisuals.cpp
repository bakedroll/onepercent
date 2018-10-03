#include "scripting/LuaVisuals.h"

namespace onep
{
  LuaVisuals::LuaVisuals(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance("visuals")
  {
  }

  LuaVisuals::~LuaVisuals()
  {
  }

  void LuaVisuals::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaVisuals>("Visuals")
      //.addFunction("debug", &LuaVisuals::lua_debug)
      .endClass();
  }
}