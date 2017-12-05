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

  std::shared_ptr<OverlayCompositor> QtGameLoadingState::getOverlayCompositor()
  {
    return m_compositor;
  }

  void QtGameLoadingState::setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor)
  {
    m_compositor = compositor;
  }
}
