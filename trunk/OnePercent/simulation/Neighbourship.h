#pragma once

#include <core/LuaStateManager.h>

namespace onep
{
  class CountryState;

  class Neighbourship : public osg::Referenced, public onep::LuaClass
  {
  public:
    typedef osg::ref_ptr<Neighbourship> Ptr;

    Neighbourship();
    ~Neighbourship();

    virtual void registerClass(lua_State* state) override;

    void addNeighbour(CountryState* state);

    // lua
    int lua_get_num_neighbours() const;
    CountryState* lua_get_neighbour_state(int i);
    void lua_set_current_branch(std::string branchName);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };

}