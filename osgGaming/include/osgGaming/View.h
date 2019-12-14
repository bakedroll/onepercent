#pragma once

#include "PostProcessingEffect.h"
#include "Hud.h"

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

    osg::ref_ptr<osg::Group> getRootGroup();
    osg::ref_ptr<Hud> getHud();
    osg::ref_ptr<osg::Camera> getSceneCamera();
    osg::ref_ptr<osg::Camera> getHudCamera();

    // TODO: REFACTOR
    osg::ref_ptr<osg::Geode> getCanvasGeode();

    void setHud(const osg::ref_ptr<Hud>& hud);

    void addPostProcessingEffect(const osg::ref_ptr<PostProcessingEffect>& ppe, bool enabled = true,
                                 const std::string& name = "");

    void setPostProcessingEffectEnabled(const std::string& ppeName, bool enabled);
    void setFullscreenEnabled(bool enabled, const osgViewer::ViewerBase::Windows& windows);
    void setWindowedResolution(const osg::Vec2f& resolution, const osgViewer::ViewerBase::Windows& windows);
    void setScreenNum(int screenNum);

    osg::ref_ptr<PostProcessingEffect> getPostProcessingEffect(const std::string& ppeName);
    bool                               getFullscreenEnabled();
    osg::Vec2f                         getResolution();
    int                                getScreenNum();

    bool getPostProcessingEffectEnabled(const std::string& ppeName);
    bool hasPostProcessingEffect(const std::string& ppeName);

    void setupResolution();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };

}