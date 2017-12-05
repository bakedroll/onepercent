#pragma once

#include <osgGaming/GameLoadingState.h>
#include <osgGaming/GameState.h>

namespace onep
{
  class OverlayCompositor;
  class VirtualOverlay;

  class QtGameLoadingState : public osgGaming::GameLoadingState
  {
  public:
    QtGameLoadingState(osg::ref_ptr<osgGaming::GameState> nextState);
    QtGameLoadingState(AbstractGameStateList nextStates);
    ~QtGameLoadingState();

    std::shared_ptr<OverlayCompositor> getOverlayCompositor();
    void setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor);

  private:
    std::shared_ptr<OverlayCompositor> m_compositor;
  };
}
