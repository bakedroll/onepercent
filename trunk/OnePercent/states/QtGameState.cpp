#include "QtGameState.h"

namespace onep
{
  QtGameState::QtGameState()
    : osgGaming::GameState()
  {
  }

  QtGameState::~QtGameState()
  {
  }

  VirtualOverlay* QtGameState::createVirtualOverlay()
  {
    return nullptr;
  }
}
