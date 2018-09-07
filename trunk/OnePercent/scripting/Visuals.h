#pragma once

#include "scripting/LuaStateManager.h"

namespace onep
{
  class Visuals : public osg::Referenced, public LuaClassInstance
  {
  public:
    Visuals(osgGaming::Injector& injector);
    ~Visuals();

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

  };
}