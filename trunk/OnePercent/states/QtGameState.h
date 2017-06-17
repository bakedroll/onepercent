#pragma once

#include <osgGaming/GameState.h>

namespace onep
{
  class VirtualOverlay;

  class QtGameState : public osgGaming::GameState
  {
  public:
    QtGameState();
    ~QtGameState();

    virtual VirtualOverlay* createVirtualOverlay();
  };
}
