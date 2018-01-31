#include "MockedWorld.h"

namespace osgGamingTest
{
  MockedWorld::MockedWorld(osgGaming::Injector& injector)
    : osgGaming::World(injector)
    , m_id(-1)
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