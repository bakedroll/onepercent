#pragma once

#include "scripting/LuaObjectMapper.h"

#include <osg/Referenced>
#include <osg/ref_ptr>

#include <map>
#include <vector>

namespace onep
{
  class Country : public osg::Referenced, public LuaObjectMapper
  {
  public:
    typedef osg::ref_ptr<Country> Ptr;
    typedef std::map<int, Ptr> Map;

    Country(const luabridge::LuaRef& object);
    ~Country();

    int getId() const;
    std::string getName() const;
    std::vector<int>& getNeighbourIds() const;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override;
    virtual void readObject(const luabridge::LuaRef& object) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}