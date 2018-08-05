#include "CountryNode.h"

#include <osg/Geometry>
#include <osgGaming/SimulationCallback.h>
#include <osgGaming/Animation.h>
#include <osgGaming/Helper.h>

#include <algorithm>
#include <assert.h>

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
    virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override
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

  struct CountryNode::Impl
  {
    Impl() {}

    osg::Vec2f centerLatLong;
    osg::Vec2f size;

    float earthRadius;
    float cameraZoom;

    List neighbors;
    BorderIdMap neighbourBorders;

    osg::ref_ptr<osg::StateSet> stateSet;

    bool bShaderEnabled;

    osg::ref_ptr<osg::Uniform> uniformColor;
    osg::ref_ptr<osg::Uniform> uniformAlpha;
    osg::ref_ptr<osg::Uniform> uniformHoverEnabled;
    osg::ref_ptr<osg::Uniform> uniformHoverIntensity;

    osg::ref_ptr<UpdateHoverIntensityCallback> callback;
  };

  CountryNode::CountryNode(
    osg::ref_ptr<ConfigManager> configManager,
    osg::Vec2f centerLatLong,
    osg::Vec2f size,
    osg::ref_ptr<osg::Vec3Array> vertices,
    osg::ref_ptr<osg::Vec2Array> texcoords1,
    osg::ref_ptr<osg::Vec3Array> texcoords2,
    osg::ref_ptr<osg::DrawElementsUInt> triangles,
    BorderIdMap& neighbourBorders)
    : osg::Geode()
    , m(new Impl())
  {
    m->earthRadius = configManager->getNumber<float>("earth.radius");
    m->cameraZoom = configManager->getNumber<float>("camera.country_zoom");

    m->centerLatLong = centerLatLong;
    m->size = size;

    m->neighbourBorders = neighbourBorders;

    osg::ref_ptr<osg::Geometry> geo = new osg::Geometry();
    geo->setVertexArray(vertices);
    geo->setTexCoordArray(0, texcoords1, osg::Array::BIND_PER_VERTEX);
    geo->setTexCoordArray(1, texcoords2, osg::Array::BIND_PER_VERTEX);
    geo->addPrimitiveSet(triangles);

    addDrawable(geo);

    m->uniformHoverEnabled = new osg::Uniform("bHoverEnabled", 0);
    m->uniformAlpha = new osg::Uniform("alpha", 0.0f);
    m->uniformColor = new osg::Uniform("color", osg::Vec3f(0.0f, 0.0f, 0.0f));
    m->uniformHoverIntensity = new osg::Uniform("hoverIntensity", 0.0f);
    
    m->stateSet = getOrCreateStateSet();
    m->stateSet->addUniform(m->uniformHoverEnabled);
    m->stateSet->addUniform(m->uniformAlpha);
    m->stateSet->addUniform(m->uniformColor);
    m->stateSet->addUniform(m->uniformHoverIntensity);

    m->bShaderEnabled = false;

    m->callback = new UpdateHoverIntensityCallback(m->uniformHoverIntensity);
    addUpdateCallback(m->callback);
  }

  CountryNode::~CountryNode()
  {
  }

  void CountryNode::addNeighbor(osg::ref_ptr<CountryNode> mesh)
  {
    m->neighbors.push_back(mesh);
  }

  CountryNode::List& CountryNode::getNeighborCountryNodes()
  {
    return m->neighbors;
  }

  const CountryNode::BorderIdMap& CountryNode::getNeighborBorders() const
  {
    return m->neighbourBorders;
  }

  const std::vector<int>& CountryNode::getNeighborBorderIds(int neighborId)
  {
    BorderIdMap::iterator it = m->neighbourBorders.find(neighborId);
    if (it == m->neighbourBorders.end())
    {
      assert(false);
      return m->neighbourBorders[neighborId];
    }

    return it->second;
  }

  bool CountryNode::getIsOnOcean() const
  {
    return m->neighbourBorders.find(-1) != m->neighbourBorders.end();
  }

  void CountryNode::setColorMode(ColorMode mode)
  {
    switch (mode)
    {
    case MODE_SELECTED:
      m->uniformAlpha->set(0.5f);
      m->uniformColor->set(osg::Vec3f(0.5f, 0.69f, 1.0f));
      break;
    case MODE_NEIGHBOR:
      m->uniformAlpha->set(0.4f);
      m->uniformColor->set(osg::Vec3f(0.5f, 0.5f, 0.5f));
      break;
    case MODE_HIGHLIGHT_BANKS:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.0f, 0.0f, 0.8f));
      break;
    case MODE_HIGHLIGHT_CONTROL:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.635f, 0.439f, 0.031f)); // #A27008
      break;
    case MODE_HIGHLIGHT_CONCERNS:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.118f, 0.753f, 0.208f)); // #1EC035
      break;
    case MODE_HIGHLIGHT_MEDIA:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.902f, 1.0f, 0.357f)); // #E6FF5B
      break;
    case MODE_HIGHLIGHT_POLITICS:
      m->uniformAlpha->set(0.3f);
      m->uniformColor->set(osg::Vec3f(0.69f, 0.247f, 0.624f));
      break;
    case MODE_HOVER:
      m->uniformAlpha->set(0.0f);
      m->uniformColor->set(osg::Vec3f(0.5f, 0.5f, 0.5f));
    default:
      break;
    }
  }

  void CountryNode::setHoverMode(bool bHoverEnabled)
  {
    int enabled;
    m->uniformHoverEnabled->get(enabled);

    if (int(bHoverEnabled) == enabled)
      return;

    m->uniformHoverEnabled->set(bHoverEnabled ? 1 : 0);
    m->callback->setEnabled(bHoverEnabled);
  }

  osg::Vec2f CountryNode::getCenterLatLong()
  {
    return m->centerLatLong;
  }

  osg::Vec2f CountryNode::getSize()
  {
    return m->size;
  }

  osg::Vec2f CountryNode::getSurfaceSize()
  {
    return osg::Vec2f(
      2.0f * C_PI * sin(C_PI / 2.0f - abs(m->centerLatLong.x())) * m->earthRadius * m->size.x(),
      C_PI * m->earthRadius * m->size.y());
  }

  float CountryNode::getOptimalCameraDistance(float angle, float ratio)
  {
    osg::Vec2f surfaceSize = getSurfaceSize();

    float hdistance = surfaceSize.x() * m->cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f));
    float vdistance = surfaceSize.y() * m->cameraZoom / (2.0f * tan(angle * C_PI / 360.0f));

    return std::max(hdistance, vdistance);
  }
}
