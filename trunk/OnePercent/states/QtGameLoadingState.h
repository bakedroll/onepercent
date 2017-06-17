#pragma once

#include <osgGaming/GameLoadingState.h>
#include <osgGaming/GameState.h>

namespace onep
{
  class VirtualOverlay;

  class QtGameLoadingState : public osgGaming::GameLoadingState
  {
  public:
    QtGameLoadingState(osg::ref_ptr<osgGaming::GameState> nextState);
    QtGameLoadingState(AbstractGameStateList nextStates);
    ~QtGameLoadingState();

    virtual VirtualOverlay* createVirtualOverlay();
  };
}
