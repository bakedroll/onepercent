#pragma once

#include "scripting/LuaObjectMapper.h"

#include <osg/Vec3f>

namespace onep
{
  class LuaModelPrototype : public LuaObjectMapper
  {
  public:
    LuaModelPrototype(const luabridge::LuaRef& object, lua_State* luaState);

    const std::string& getFilename() const;
    const osg::Vec3f&  getScale() const;
    const osg::Vec3f&  getRotationDeg() const;
    const osg::Vec3f&  getPosition() const;

  private:
    std::string m_filename;
    osg::Vec3f  m_scale;
    osg::Vec3f  m_rotationDeg;
    osg::Vec3f  m_position;

  };
}  // namespace onep