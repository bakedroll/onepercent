#include "osgGaming/Viewer.h"
#include <osgGaming/View.h>

#include <cassert>

namespace osgGaming
{
  Viewer::Viewer()
    : osgViewer::CompositeViewer()
  {
    setThreadingModel(SingleThreaded);
  }

  void Viewer::setFullscreenEnabled(int viewId, bool enabled)
  {
    auto view = getGamingView(viewId);
    if (!view.valid())
    {
      return;
    }

    Windows windows;
    getWindows(windows);
    view->setFullscreenEnabled(enabled, windows);
  }

  void Viewer::setWindowedResolution(int viewId, const osg::Vec2f& resolution)
  {
    auto view = getGamingView(viewId);
    if (!view.valid())
    {
      return;
    }

    Windows windows;
    getWindows(windows);
    view->setWindowedResolution(resolution, windows);
  }

  osg::ref_ptr<osgGaming::View> Viewer::getGamingView(unsigned int i)
  {
    auto view = dynamic_cast<osgGaming::View*>(osgViewer::CompositeViewer::getView(i));
    assert(view);
    return view;
  }
}