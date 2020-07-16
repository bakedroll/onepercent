#include "scripting/LuaModelPrototype.h"
#include "core/Helper.h"

onep::LuaModelPrototype::LuaModelPrototype(const luabridge::LuaRef& object, lua_State* luaState)
  : LuaObjectMapper(object, luaState), m_scale(1.0f, 1.0f, 1.0f)
{
  m_filename = getString("filename");

  luabridge::LuaRef refScale = object["scale"];
  if (!refScale.isNil())
  {
    m_scale = Helper::parseOsgVecFromLua<osg::Vec3f>(refScale);
  }

  luabridge::LuaRef refRotation = object["rotation"];
  if (!refRotation.isNil())
  {
    m_rotationDeg = Helper::parseOsgVecFromLua<osg::Vec3f>(refRotation);
  }

  luabridge::LuaRef refPosition = object["position"];
  if (!refPosition.isNil())
  {
    m_position = Helper::parseOsgVecFromLua<osg::Vec3f>(refPosition);
  }
}

const std::string& onep::LuaModelPrototype::getFilename() const
{
  return m_filename;
}

const osg::Vec3f& onep::LuaModelPrototype::getScale() const
{
  return m_scale;
}

const osg::Vec3f& onep::LuaModelPrototype::getRotationDeg() const
{
  return m_rotationDeg;
}

const osg::Vec3f& onep::LuaModelPrototype::getPosition() const
{
  return m_position;
}
