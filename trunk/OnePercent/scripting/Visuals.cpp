#include "scripting/Visuals.h"

namespace onep
{
  Visuals::Visuals(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance()
  {
  }

  Visuals::~Visuals()
  {
  }

  void Visuals::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<Visuals>("Visuals")
      //.addFunction("debug", &Visuals::lua_debug)
      .endClass();
  }

  std::string Visuals::instanceVariableName()
  {
    return "visuals";
  }
}