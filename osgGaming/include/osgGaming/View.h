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

    void updateResolution(osg::Vec2f resolution);
    void updateWindowPosition(osg::Vec2f position);
    virtual void setSceneData(osg::Node* node) override;

    void setClampColorEnabled(bool enabled);

    osg::ref_ptr<osg::Group> getRootGroup();
    osg::ref_ptr<Hud> getHud();
    osg::ref_ptr<osg::Camera> getSceneCamera();
    osg::ref_ptr<osg::Camera> getHudCamera();

    // TODO: REFACTOR
    osg::ref_ptr<osg::Geode> getCanvasGeode();

    void setHud(osg::ref_ptr<Hud> hud);

    void addPostProcessingEffect(osg::ref_ptr<PostProcessingEffect> ppe, bool enabled = true, std::string name = "");

    void setPostProcessingEffectEnabled(std::string ppeName, bool enabled);
    void setPostProcessingEffectEnabled(unsigned int index, bool enabled);
    void setFullscreenEnabled(bool enabled, const osgViewer::ViewerBase::Windows& windows);
    void setWindowedResolution(osg::Vec2f resolution, const osgViewer::ViewerBase::Windows& windows);
    void setScreenNum(int screenNum);

    osg::ref_ptr<PostProcessingEffect> getPostProcessingEffect(std::string ppeName);
    osg::ref_ptr<PostProcessingEffect> getPostProcessingEffect(unsigned int index);
    bool getFullscreenEnabled();
    osg::Vec2f getResolution();
    int getScreenNum();

    bool getPostProcessingEffectEnabled(std::string ppeName);
    bool getPostProcessingEffectEnabled(unsigned int index);

    bool hasPostProcessingEffect(std::string ppeName);

    void setupResolution();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
  };

}