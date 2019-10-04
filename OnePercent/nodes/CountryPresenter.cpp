#include "nodes/CountryPresenter.h"
#include "core/Macros.h"

#include <osg/MatrixTransform>

#include <osgGaming/Helper.h>

namespace onep
{
  float calcSegmentArea(float radius, float r1, float r2)
  {
      auto radius2 = radius * radius;
      return 2.0f * C_PI * radius * (sqrt(radius2 - (r2 * r2)) - sqrt(radius2 - (r1 * r1)));
  }

  float rand01()
  {
    return static_cast<float>(rand()) / RAND_MAX;
  }

  void CountryPresenter::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
            .beginClass<CountryPresenter>("CountryPresenter")
            .addFunction("add_node", &CountryPresenter::luaAddNode)
            .addFunction("add_node_to_bin", &CountryPresenter::luaAddNodeToBin)
            .addFunction("add_node_to_bin_at", &CountryPresenter::luaAddNodeToBinAt)
            .addFunction("scatter_nodes_to_bin", &CountryPresenter::luaScatterNodesToBin)
            .addFunction("remove_node_bin", &CountryPresenter::luaRemoveNodeBin)
            .addFunction("clear_nodes", &CountryPresenter::luaClearNodes)
            .endClass();
  }

  CountryPresenter::CountryPresenter(const LuaConfig::Ptr&                configManager,
                                     const std::shared_ptr<CountriesMap>& countriesMap, const osg::Vec2f& centerLatLong,
                                     const osg::Vec2f& size)
    : m_centerLatLong(centerLatLong),
      m_size(size),
      m_earthRadius(configManager->getNumber<float>("earth.radius")),
      m_cameraZoom(configManager->getNumber<float>("camera.country_zoom")),
      m_countriesMap(countriesMap)
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
    luaAddNodeToBin(node, "");
  }

  void CountryPresenter::luaAddNodeToBin(osg::Node* node, const std::string& nodeBin)
  {
    luaAddNodeToBinAt(node, nodeBin, osg::Vec2f(0.5f, 0.5f));
  }

  void CountryPresenter::luaAddNodeToBinAt(osg::Node* node, const std::string& nodeBin, const osg::Vec2f& relPosition)
  {
    m_transformBins[nodeBin].push_back(addTransformFromNode(node, relPosition));
  }

  void CountryPresenter::luaScatterNodesToBin(osg::Node* node, const std::string& nodeBin, float density)
  {
    auto count = static_cast<int>(getSurfaceArea() * density);
    for (auto i=0; i<count; i++)
    {
      osg::Vec2f relPosition(rand01(), rand01());
      luaAddNodeToBinAt(node, nodeBin, relPosition);
    }
  }

  void CountryPresenter::luaRemoveNodeBin(const std::string& nodeBin)
  {
    if (m_transformBins.count(nodeBin) == 0)
    {
      OSGG_QLOG_WARN(QString("Node bin '%1' not found").arg(nodeBin.c_str()));
      return;
    }

    auto it = m_transformBins.find(nodeBin);
    for (const auto& transform : it->second)
    {
      removeChild(transform);
    }
    m_transformBins.erase(it);
  }

  void CountryPresenter::luaClearNodes()
  {
    m_transformBins.clear();

    const auto numChilds = getNumChildren();
    for (auto i = 0U; i < numChilds; i++)
    {
      removeChild(getChild(i));
    }
  }

  float CountryPresenter::getSurfaceArea() const
  {
    auto alpha  = m_centerLatLong.x();
    auto height = m_size.y() * C_PI * 0.5f;

    auto a1 = alpha + height;
    auto a2 = alpha - height;
    auto r1 = m_earthRadius * cos(a1);
    auto r2 = m_earthRadius * cos(a2);

    if (signbit(a1) == signbit(a2))
    {
      if (r1 < r2)
      {
        std::swap(r1, r2);
      }

      return calcSegmentArea(m_earthRadius, r1, r2) * m_size.x();
    }

    return (calcSegmentArea(m_earthRadius, m_earthRadius, std::abs(r1)) +
            calcSegmentArea(m_earthRadius, m_earthRadius, std::abs(r2))) *
           m_size.x();
  }

  CountryPresenter::MatrixTransformPtr CountryPresenter::addTransformFromNode(osg::Node* node, const osg::Vec2f& relPosition)
  {
    auto offsetLat  = m_size.x() * (relPosition.x() - 0.5f) * 2.0f * C_PI;
    auto offsetLong = m_size.y() * (relPosition.y() - 0.5f) * C_PI;

    auto matRot = osg::Matrix::rotate(osgGaming::getQuatFromEuler(
            offsetLong - m_centerLatLong.x(), 0.0f, fmodf(m_centerLatLong.y() + C_PI + offsetLat, C_PI * 2.0f)));

    auto matMove = osg::Matrix::translate(osg::Vec3f(0.0f, -m_earthRadius, 0.0f));

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
    transform->setMatrix(matMove * matRot);

    transform->addChild(node);
    addChild(transform);

    return transform;
  }
}
