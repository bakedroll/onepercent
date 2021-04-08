#include "CountryHoverNode.h"

#include "nodes/CountryGeometry.h"

#include <osgHelper/SimulationCallback.h>
#include <osgHelper/Animation.h>

namespace onep
{
  class UpdateHoverIntensityCallback : public osgHelper::SimulationCallback
  {
  public:
    UpdateHoverIntensityCallback(const osg::ref_ptr<osg::Uniform>& u)
      : osgHelper::SimulationCallback()
      , uniform(u)
      , animationHover(new osgHelper::Animation<float>(0.0f, 0.2f, osgHelper::AnimationEase::CIRCLE_OUT))
      , bEnabled(false)
      , bStarted(false)
    {}

    void setEnabled(bool e)
    {
      if (bEnabled == e)
        return;

      bEnabled = e;
      if (bEnabled)
        bStarted = true;

    }

  protected:
    void action(const SimulationData& data) override
    {
      if (!bEnabled)
        return;

      if (bStarted)
      {
        bStarted = false;
        animationHover->beginAnimation(0.0f, 1.0f, data.time);
      }

      uniform->set(animationHover->getValue(data.time));
    }

  private:
    osg::ref_ptr<osg::Uniform> uniform;
    osg::ref_ptr<osgHelper::Animation<float>> animationHover;
    bool bEnabled;
    bool bStarted;
  };

  struct CountryHoverNode::Impl
  {
    Impl() = default;
    ~Impl() = default;

    osg::ref_ptr<osg::Uniform> uniformAlpha;
    osg::ref_ptr<osg::Uniform> uniformHoverIntensity;

    osg::ref_ptr<osg::StateSet> stateSet;

    osg::ref_ptr<UpdateHoverIntensityCallback> callback;
  };

  CountryHoverNode::CountryHoverNode(
    const osg::ref_ptr<osg::Vec3Array>& vertices,
      const osg::ref_ptr<osg::Vec2Array>& texcoords,
      const osg::ref_ptr<osg::DrawElementsUInt>& triangles)
    : osg::Geode()
    , m(new Impl())
  {
    const auto geo = new CountryGeometry(vertices, triangles, { texcoords });
    Geode::addDrawable(geo);

    m->uniformAlpha = new osg::Uniform("alpha", 0.0f);
    m->uniformHoverIntensity = new osg::Uniform("hoverIntensity", 0.0f);

    m->stateSet = getOrCreateStateSet();
    m->stateSet->addUniform(m->uniformAlpha);
    m->stateSet->addUniform(m->uniformHoverIntensity);

    m->callback = new UpdateHoverIntensityCallback(m->uniformHoverIntensity);
    addUpdateCallback(m->callback);
  }

  CountryHoverNode::~CountryHoverNode() = default;

  void CountryHoverNode::setHoverEnabled(bool bEnabled)
  {
    m->callback->setEnabled(bEnabled);
  }
}