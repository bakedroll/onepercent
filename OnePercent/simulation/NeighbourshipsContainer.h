#pragma once

#include "scripting/LuaStateManager.h"
#include "simulation/Neighbourship.h"

#include <osg/Referenced>

namespace onep
{
  class CountryState;

  class NeighbourshipsContainer : public osg::Referenced, public LuaClassInstance
  {
  public:
    typedef osg::ref_ptr<NeighbourshipsContainer> Ptr;

    NeighbourshipsContainer(osgGaming::Injector& injector);
    ~NeighbourshipsContainer();

    virtual void registerClass(lua_State* state) override;
    virtual std::string instanceVariableName() override;

    void prepare();

    // lua
    Neighbourship* lua_get_neighbourship(int country_id);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };

}