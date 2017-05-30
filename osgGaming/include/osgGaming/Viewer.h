#pragma once

#include <osgViewer/CompositeViewer>

namespace osgGaming
{
  class View;

  class Viewer : public osgViewer::CompositeViewer
  {
  public:
    Viewer();

    void setFullscreenEnabled(int viewId, bool enabled);
    void setWindowedResolution(int viewId, osg::Vec2f resolution);

    osg::ref_ptr<View> getView(unsigned int i);
  };
}