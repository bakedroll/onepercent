#include "MockedGameApplication.h"

#include <osgGaming/InputManager.h>

namespace osgGamingTest
{
  MockedGameApplication::MockedGameApplication()
    : GameApplication()
  {
    osg::ref_ptr<osgGaming::View> view = new osgGaming::View();
    view->setupResolution();

    getViewer()->addView(view);
  }

  MockedGameApplication::~MockedGameApplication()
  {
  }

  int MockedGameApplication::runGame(osgGaming::AbstractGameState::AbstractGameStateRefList initialStates)
  {
    return osgGaming::GameApplication::runGame(initialStates);
  }

  int MockedGameApplication::mainloop()
  {
    double simTime = 0.0;
    while (!isGameEnded())
    {
      action(new osg::Node(), new osg::NodeVisitor(), simTime, 0.0);
      simTime++;
    }

    return 0;
  }

  osg::ref_ptr<osgGaming::InputManager> MockedGameApplication::createInputManager(osg::ref_ptr<osgGaming::View> view)
  {
    return new osgGaming::InputManager();
  }
}