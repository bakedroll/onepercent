#include "osgGaming/Viewer.h"
#include <osgGaming/View.h>

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
    View* gview = dynamic_cast<View*>(view);

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
    View* gview = dynamic_cast<View*>(view);

    if (!gview)
    {
      assert(false && "Should be a osgGaming::View*");
      return;
    }

    gview->setWindowedResolution(resolution, windows);
  }

  osg::ref_ptr<View> Viewer::getView(unsigned i)
  {
    View::Ptr view = dynamic_cast<View*>(osgViewer::CompositeViewer::getView(i));
    assert(view);
    return view;
  }
}