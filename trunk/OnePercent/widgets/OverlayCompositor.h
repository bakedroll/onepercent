#pragma once

#include <QWidget>

#include <osg/Group>

#include <memory>

namespace onep
{
  class VirtualOverlay;

  class OverlayCompositor
  {
  public:
    OverlayCompositor();
    ~OverlayCompositor();

    void setRootWidget(QWidget* widget);

    void addVirtualOverlay(VirtualOverlay* overlay);
    void removeVirtualOverlay(VirtualOverlay* overlay);

    void clear();

    osg::ref_ptr<osg::Group> getContainer();

    void renderVirtualOverlays();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };
}