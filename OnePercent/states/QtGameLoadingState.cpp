#include "QtGameLoadingState.h"

namespace onep
{
  QtGameLoadingState::QtGameLoadingState(osg::ref_ptr<osgGaming::GameState> nextState)
    : osgGaming::GameLoadingState(nextState)
  {
  }

  QtGameLoadingState::QtGameLoadingState(AbstractGameStateList nextStates)
    : osgGaming::GameLoadingState(nextStates)
  {
  }

  QtGameLoadingState::~QtGameLoadingState()
  {
  }

  VirtualOverlay* QtGameLoadingState::createVirtualOverlay()
  {
    return nullptr;
  }
}
