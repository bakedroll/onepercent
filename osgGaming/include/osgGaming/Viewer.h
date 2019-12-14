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
    void setWindowedResolution(int viewId, const osg::Vec2f& resolution);

    osg::ref_ptr<osgGaming::View> getGamingView(unsigned int i);
  };
}