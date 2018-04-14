#pragma once

#include <osg/Referenced>

#include <osgGaming/Injector.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class Country;

  class CountriesContainer : public osg::Referenced
  {
  public:
    typedef osg::ref_ptr<CountriesContainer> Ptr;

    CountriesContainer(osgGaming::Injector& injector);
    ~CountriesContainer();

    osg::ref_ptr<Country> getCountry(int id);

    void loadFromLua(const luabridge::LuaRef object);
    void writeToLua();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}