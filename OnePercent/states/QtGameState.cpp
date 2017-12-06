#include "QtGameState.h"

namespace onep
{
  struct QtGameState::Impl
  {
    Impl() {}

    std::shared_ptr<OverlayCompositor> compositor;
    osgGaming::Observable<bool>::Ptr oFullscreenEnabled;
  };

  QtGameState::QtGameState()
    : osgGaming::GameState()
    , m(new Impl())
  {
  }

  QtGameState::~QtGameState()
  {
  }

  osgGaming::Observable<bool>::Ptr QtGameState::getFullscreenEnabledObs()
  {
    return m->oFullscreenEnabled;
  }

  std::shared_ptr<OverlayCompositor> QtGameState::getOverlayCompositor()
  {
    return m->compositor;
  }

  void QtGameState::setFullscreenEnabledObs(osgGaming::Observable<bool>::Ptr oFullscreenEnabled)
  {
    m->oFullscreenEnabled = oFullscreenEnabled;
  }

  void QtGameState::setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor)
  {
    m->compositor = compositor;
  }
}
