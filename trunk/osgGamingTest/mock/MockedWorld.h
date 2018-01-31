#pragma once

#include <osgGaming/World.h>

namespace osgGamingTest
{
  class MockedWorld : public osgGaming::World
  {
  public:
    MockedWorld(osgGaming::Injector& injector);

    int getId();
    void setId(int id);

  private:
    int m_id;
  };
}