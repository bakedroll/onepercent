#include "QtGameLoadingState.h"

namespace onep
{
  struct QtGameLoadingState::Impl
  {
    Impl() {}

    std::shared_ptr<OverlayCompositor> compositor;
    osgGaming::Observable<bool>::Ptr oFullscreenEnabled;
  };

  QtGameLoadingState::QtGameLoadingState(osg::ref_ptr<osgGaming::GameState> nextState)
    : osgGaming::GameLoadingState(nextState)
    , m(new Impl())
  {
  }

  QtGameLoadingState::QtGameLoadingState(AbstractGameStateList nextStates)
    : osgGaming::GameLoadingState(nextStates)
    , m(new Impl())
  {
  }

  QtGameLoadingState::~QtGameLoadingState()
  {
  }

  osgGaming::Observable<bool>::Ptr QtGameLoadingState::getFullscreenEnabledObs()
  {
    return m->oFullscreenEnabled;
  }

  std::shared_ptr<OverlayCompositor> QtGameLoadingState::getOverlayCompositor()
  {
    return m->compositor;
  }

  void QtGameLoadingState::setFullscreenEnabledObs(osgGaming::Observable<bool>::Ptr oFullscreenEnabled)
  {
    m->oFullscreenEnabled = oFullscreenEnabled;
  }

  void QtGameLoadingState::setOverlayCompositor(std::shared_ptr<OverlayCompositor> compositor)
  {
    m->compositor = compositor;
  }
}