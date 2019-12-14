#pragma once

#include "osgGaming/PostProcessingEffect.h"
#include "osgGaming/Hud.h"

#include <osgViewer/ViewerBase>
#include <osgViewer/View>

#include <memory>

namespace osgGaming
{
  class View : public osgViewer::View
  {
  public:
    typedef osg::ref_ptr<View> Ptr;
    typedef osg::observer_ptr<View> WeakPtr;

    View();
    ~View();

    void updateResolution(const osg::Vec2f& resolution);
    void updateWindowPosition(const osg::Vec2f& position);
    void setRootNode(const osg::ref_ptr<osg::Node>& node);

    void setClampColorEnabled(bool enabled);

    osg::ref_ptr<osg::Group> getRootGroup() const;
    osg::ref_ptr<Hud> getHud() const;
    osg::ref_ptr<osg::Camera> getSceneCamera() const;
    osg::ref_ptr<osg::Camera> getHudCamera() const;

    // TODO: REFACTOR
    osg::ref_ptr<osg::Geode> getCanvasGeode() const;

    void setHud(const osg::ref_ptr<Hud>& hud);

    void addPostProcessingEffect(const osg::ref_ptr<PostProcessingEffect>& ppe, bool enabled = true,
                                 const std::string& name = "");

    void setPostProcessingEffectEnabled(const std::string& ppeName, bool enabled);
    void setFullscreenEnabled(bool enabled, const osgViewer::ViewerBase::Windows& windows);
    void setWindowedResolution(const osg::Vec2f& resolution, const osgViewer::ViewerBase::Windows& windows);
    void setScreenNum(int screenNum);

    osg::ref_ptr<PostProcessingEffect> getPostProcessingEffect(const std::string& ppeName) const;
    bool                               getFullscreenEnabled() const;
    osg::Vec2f                         getResolution() const;
    int                                getScreenNum() const;

    bool getPostProcessingEffectEnabled(const std::string& ppeName) const;
    bool hasPostProcessingEffect(const std::string& ppeName) const;

    void setupResolution();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };

}