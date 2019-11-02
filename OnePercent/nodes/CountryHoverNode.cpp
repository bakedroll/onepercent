#include "CountryHoverNode.h"

#include "nodes/CountryGeometry.h"

#include <osgGaming/SimulationCallback.h>
#include <osgGaming/Animation.h>

namespace onep
{
  class UpdateHoverIntensityCallback : public osgGaming::SimulationCallback
  {
  public:
    UpdateHoverIntensityCallback(osg::ref_ptr<osg::Uniform> u)
      : osgGaming::SimulationCallback()
      , uniform(u)
      , animationHover(new osgGaming::Animation<float>(0.0f, 0.2f, osgGaming::AnimationEase::CIRCLE_OUT))
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
    void action(osg::Object* object, osg::Object* data, double simTime, double timeDiff) override
    {
      if (!bEnabled)
        return;

      if (bStarted)
      {
        bStarted = false;
        animationHover->beginAnimation(0.0f, 1.0f, simTime);
      }

      uniform->set(animationHover->getValue(simTime));
    }

  private:
    osg::ref_ptr<osg::Uniform> uniform;
    osg::ref_ptr<osgGaming::Animation<float>> animationHover;
    bool bEnabled;
    bool bStarted;
  };

  struct CountryHoverNode::Impl
  {
    Impl() {}
    ~Impl() {}

    osg::ref_ptr<osg::Uniform> uniformAlpha;
    osg::ref_ptr<osg::Uniform> uniformHoverIntensity;

    osg::ref_ptr<osg::StateSet> stateSet;

    osg::ref_ptr<UpdateHoverIntensityCallback> callback;
  };

  CountryHoverNode::CountryHoverNode(
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::Vec2Array> texcoords,
    osg::ref_ptr<osg::DrawElementsUInt> triangles)
    : osg::Geode()
    , m(new Impl())
  {
    osg::ref_ptr<CountryGeometry> geo = new CountryGeometry(vertices, texcoords, nullptr, triangles);
    addDrawable(geo);

    m->uniformAlpha = new osg::Uniform("alpha", 0.0f);
    m->uniformHoverIntensity = new osg::Uniform("hoverIntensity", 0.0f);

    m->stateSet = getOrCreateStateSet();
    m->stateSet->addUniform(m->uniformAlpha);
    m->stateSet->addUniform(m->uniformHoverIntensity);

    m->callback = new UpdateHoverIntensityCallback(m->uniformHoverIntensity);
    addUpdateCallback(m->callback);
  }

  CountryHoverNode::~CountryHoverNode()
  {
  }

  void CountryHoverNode::setHoverEnabled(bool bEnabled)
  {
    m->callback->setEnabled(bEnabled);
  }
}