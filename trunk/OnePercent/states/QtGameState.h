#pragma once

#include <osgGaming/GameState.h>
#include <osgGaming/Observable.h>

namespace onep
{
  class OverlayCompositor;
  class VirtualOverlay;

  class QtGameState : public osgGaming::GameState
  {
  public:
    QtGameState();
    ~QtGameState();

    osgGaming::Observable<bool>::Ptr getFullscreenEnabledObs();
    std::shared_ptr<OverlayCompositor> getOverlayCompositor();

    void setFullscreenEnabledObs(osgGaming::Observable<bool>::Ptr oFullscreenEnabled);
    void setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}
