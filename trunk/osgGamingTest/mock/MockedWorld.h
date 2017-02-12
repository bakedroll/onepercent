#pragma once

#include <osgGaming/World.h>

namespace osgGamingTest
{
  class MockedWorld : public osgGaming::World
  {
  public:
    MockedWorld(int id = -1);

    int getId();
    void setId(int id);

  private:
    int m_id;
  };
}