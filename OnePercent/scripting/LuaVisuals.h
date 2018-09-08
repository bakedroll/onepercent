#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class LuaVisuals : public osg::Referenced, public LuaClassInstance
  {
  public:
    LuaVisuals(osgGaming::Injector& injector);
    ~LuaVisuals();

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

  };
}