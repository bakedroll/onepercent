#include "MockedWorld.h"

namespace osgGamingTest
{
  MockedWorld::MockedWorld(int id)
    : osgGaming::World()
    , m_id(id)
  {
  }

  int MockedWorld::getId()
  {
    return m_id;
  }

  void MockedWorld::setId(int id)
  {
    m_id = id;
  }
}