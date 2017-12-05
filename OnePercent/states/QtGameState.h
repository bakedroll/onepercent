#pragma once

#include <osgGaming/GameState.h>

namespace onep
{
  class OverlayCompositor;
  class VirtualOverlay;

  class QtGameState : public osgGaming::GameState
  {
  public:
    QtGameState();
    ~QtGameState();

    std::shared_ptr<OverlayCompositor> getOverlayCompositor();
    void setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor);

  private:
    std::shared_ptr<OverlayCompositor> m_compositor;
  };
}
