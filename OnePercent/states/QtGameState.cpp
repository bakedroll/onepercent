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

  std::shared_ptr<OverlayCompositor> QtGameState::getOverlayCompositor()
  {
    return m_compositor;
  }

  void QtGameState::setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor)
  {
    m_compositor = compositor;
  }
}
