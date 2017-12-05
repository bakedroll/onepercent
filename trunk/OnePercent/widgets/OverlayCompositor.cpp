#include "OverlayCompositor.h"

#include "widgets/VirtualOverlay.h"

#include <QGridLayout>

namespace onep
{
  struct OverlayCompositor::Impl
  {
    typedef std::set<VirtualOverlay*> OverlaySet;

    Impl()
      : rootWidget(nullptr)
      , container(new osg::Group())
      , firstFrame(GL_TRUE)
    {}

    void deleteWidget(QWidget* widget)
    {
      if (widget->layout())
        deleteLayout(widget->layout());

      widget->hide();
      delete widget;
    }

    void deleteLayout(QLayout* layout)
    {
      QLayoutItem* item;
      QLayout* sublayout;
      QWidget* widget;
      while ((item = layout->takeAt(0)))
      {
        if ((sublayout = item->layout()) != nullptr)
        {
          deleteLayout(sublayout);
        }
        else if ((widget = item->widget()) != nullptr)
        {
          widget->hide();
          delete widget;
        }
        else
        {
          delete item;
        }
      }

      delete layout;
    }

    QWidget* rootWidget;

    osg::ref_ptr<osg::Group> container;
    OverlaySet overlays;

    bool firstFrame;
  };

  OverlayCompositor::OverlayCompositor()
    : m(new Impl())
  {
  }

  OverlayCompositor::~OverlayCompositor()
  {
  }

  void OverlayCompositor::setRootWidget(QWidget* widget)
  {
    m->rootWidget = widget;
  }

  void OverlayCompositor::addVirtualOverlay(VirtualOverlay* overlay)
  {
    if (m->overlays.find(overlay) != m->overlays.end())
      return;

    m->container->addChild(overlay->getOsgNode());

    overlay->setParent(m->rootWidget);
    //overlay->show();

    m->overlays.insert(overlay);
  }

  void OverlayCompositor::removeVirtualOverlay(VirtualOverlay* overlay)
  {
    Impl::OverlaySet::iterator it = m->overlays.find(overlay);
    if (it == m->overlays.end())
      return;

    m->container->removeChild(overlay->getOsgNode());
    overlay->setParent(nullptr);
    m->deleteWidget(overlay);

    m->overlays.erase(it);
  }

  void OverlayCompositor::clear()
  {
    for (Impl::OverlaySet::iterator it = m->overlays.begin(); it != m->overlays.end(); ++it)
    {
      VirtualOverlay* overlay = *it;

      m->container->removeChild(overlay->getOsgNode());

      overlay->setParent(nullptr);
      m->deleteWidget(overlay);
    }

    m->overlays.clear();
    m->firstFrame = true;
  }

  osg::ref_ptr<osg::Group> OverlayCompositor::getContainer()
  {
    return m->container;
  }

  void OverlayCompositor::renderVirtualOverlays()
  {
    if (m->firstFrame)
    {
      for (Impl::OverlaySet::iterator it = m->overlays.begin(); it != m->overlays.end(); ++it)
      {
        if ((*it)->shouldBeVisible())
          (*it)->show();
      }

      m->firstFrame = false;
    }

    for (Impl::OverlaySet::iterator it = m->overlays.begin(); it != m->overlays.end(); ++it)
    {
      (*it)->renderToTexture();
    }
  }
}