#include "scripting/LuaExternalClassDefinitions.h"

#include <LuaBridge/LuaBridge.h>

#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec4d>

namespace onep
{
  void LuaExternalClassDefinitions::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<osg::Vec2f>("Vec2f")
      .addConstructor<void (*) (float, float)>()
      .endClass();

    luabridge::getGlobalNamespace(state)
      .beginClass<osg::Vec3f>("Vec3f")
      .addConstructor<void (*) (float, float, float)>()
      .endClass();

    luabridge::getGlobalNamespace(state)
      .beginClass<osg::Vec4f>("Vec4f")
      .addConstructor<void (*) (float, float, float, float)>()
      .endClass();

    luabridge::getGlobalNamespace(state)
      .beginClass<osg::Vec2d>("Vec2d")
      .addConstructor<void (*) (double, double)>()
      .endClass();

    luabridge::getGlobalNamespace(state)
      .beginClass<osg::Vec3d>("Vec3d")
      .addConstructor<void (*) (double, double, double)>()
      .endClass();

    luabridge::getGlobalNamespace(state)
      .beginClass<osg::Vec4d>("Vec4d")
      .addConstructor<void (*) (double, double, double, double)>()
      .endClass();
  }
}
