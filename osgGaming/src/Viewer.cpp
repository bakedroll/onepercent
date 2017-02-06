#include "osgGaming/Viewer.h"
#include <osgGaming/NativeView.h>

#include <assert.h>

namespace osgGaming
{
  Viewer::Viewer()
    : osgViewer::CompositeViewer()
  {
    setThreadingModel(SingleThreaded);
  }

  void Viewer::setFullscreenEnabled(int viewId, bool enabled)
  {
    Windows windows;
    getWindows(windows);

    osgViewer::View* view = getView(viewId);
    NativeView* gview = dynamic_cast<NativeView*>(view);

    if (!gview)
    {
      assert(false && "Should be a osgGaming::View*");
      return;
    }

    gview->setFullscreenEnabled(enabled, windows);
  }

  void Viewer::setWindowedResolution(int viewId, osg::Vec2f resolution)
  {
    Windows windows;
    getWindows(windows);

    osgViewer::View* view = getView(viewId);
    NativeView* gview = dynamic_cast<NativeView*>(view);

    if (!gview)
    {
      assert(false && "Should be a osgGaming::View*");
      return;
    }

    gview->setWindowedResolution(resolution, windows);
  }
}