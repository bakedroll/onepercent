#include "nodes/CountryPresenter.h"

#include <osg/MatrixTransform>

#include <osgGaming/Helper.h>

namespace onep
{
  void CountryPresenter::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .beginClass<CountryPresenter>("CountryPresenter")
      .addFunction("add_node", &CountryPresenter::luaAddNode)
      .addFunction("clear_nodes", &CountryPresenter::luaClearNodes)
      .endClass();
  }

  CountryPresenter::CountryPresenter(const LuaConfig::Ptr& configManager, const osg::Vec2f& centerLatLong,
                                     const osg::Vec2f& size)
	  : m_centerLatLong(centerLatLong)
    , m_size(size)
    , m_earthRadius(configManager->getNumber<float>("earth.radius"))
    , m_cameraZoom(configManager->getNumber<float>("camera.country_zoom"))
  {
  }

  osg::Vec2f CountryPresenter::getCenterLatLong() const
  {
    return m_centerLatLong;
  }

  osg::Vec2f CountryPresenter::getSize() const
  {
    return m_size;
  }

  osg::Vec2f CountryPresenter::getSurfaceSize() const
  {
    return osg::Vec2f(
      2.0f * C_PI * sin(C_PI / 2.0f - abs(m_centerLatLong.x())) * m_earthRadius * m_size.x(),
      C_PI * m_earthRadius * m_size.y());
  }

  float CountryPresenter::getOptimalCameraDistance(float angle, float ratio) const
  {
    osg::Vec2f surfaceSize = getSurfaceSize();

    float hdistance = surfaceSize.x() * m_cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f));
    float vdistance = surfaceSize.y() * m_cameraZoom / (2.0f * tan(angle * C_PI / 360.0f));

    return std::max(hdistance, vdistance);
  }

  void CountryPresenter::luaAddNode(osg::Node* node)
  {
    auto matRot = osg::Matrix::rotate(
            osgGaming::getQuatFromEuler(-m_centerLatLong.x(), 0.0f, fmodf(m_centerLatLong.y() + C_PI, C_PI * 2.0f)));

    auto matMove = osg::Matrix::translate(osg::Vec3f(0.0f, -m_earthRadius, 0.0f));

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
    transform->setMatrix(matMove * matRot);

    transform->addChild(node);
    addChild(transform);
  }

  void CountryPresenter::luaClearNodes()
  {
    const auto numChilds = getNumChildren();
    for (auto i = 0U; i < numChilds; i++)
    {
      removeChild(getChild(i));
    }
  }
}
