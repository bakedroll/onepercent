#include "nodes/CountryPresenter.h"
#include "data/CountriesMap.h"
#include "core/Macros.h"

#include <cmath>

#include <QString>

#include <osg/MatrixTransform>

#include <osgGaming/Helper.h>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>

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

  CountryPresenter::CountryPresenter(int id, const LuaConfig::Ptr& configManager,
                                     const std::shared_ptr<CountriesMap>& countriesMap, const osg::Vec2f& centerLatLong,
                                     const osg::Vec2f& size)
    : m_id(id),
      m_centerLatLong(centerLatLong),
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
    addTransformFromNodeToCenter(node, nodeBin, relPosition);
  }

  void CountryPresenter::luaScatterNodesToBin(osg::Node* node, const std::string& nodeBin, float density)
  {
    const auto pi2              = C_PI * 2.0f;
    const auto halfPi           = C_PI / 2.0f;
    const auto latCoverage      = m_size.y() * C_PI;
    const auto halfLatCoverage  = latCoverage * 0.5f;
    const auto halfLongCoverage = m_size.x() * C_PI;
    const auto longCoverage     = halfLongCoverage * 2.0f;
    const auto latMin           = m_centerLatLong.x() - halfLatCoverage;
    const auto latMax           = m_centerLatLong.x() + halfLongCoverage;
    const auto n                = static_cast<int>(latCoverage * longCoverage * 1000.0f * density);

    auto maxLongRange = 1.0f;
    if (latMin >= 0.0f || latMax <= 0.0f)
    {
      maxLongRange = std::max(cos(latMin), cos(latMax));
    }

    for (auto i = 0; i < n; i++)
    {
      const auto latitude = latMin + rand01() * latCoverage;
      const auto longRange = cos(latitude);

      const auto longitude = m_centerLatLong.y() + (rand01() - 0.5f) * longCoverage * (maxLongRange / longRange);

      osg::Vec2f coords(osg::clampBetween(fmodf(longitude / pi2 + 0.5f, 1.0f), 0.0f, 1.0f),
                        1.0f - osg::clampBetween((latitude + halfPi) / C_PI, 0.0f, 1.0f));

      if (static_cast<int>(m_countriesMap->getDataAt(coords)) != m_id)
      {
        continue;
      }

      addTransformFromNodeToCoords(node, nodeBin, osg::Vec2f(-latitude, fmodf(longitude + C_PI, pi2)));
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

    if (std::signbit(a1) == std::signbit(a2))
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

  void CountryPresenter::addTransformFromNodeToCoords(osg::Node* node, const std::string& nodeBin,
                                                      const osg::Vec2f& latLong)
  {
    auto matRot  = osg::Matrix::rotate(osgGaming::getQuatFromEuler(latLong.x(), 0.0f, latLong.y()));
    auto matMove = osg::Matrix::translate(osg::Vec3f(0.0f, -m_earthRadius, 0.0f));

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
    transform->setMatrix(matMove * matRot);

    transform->addChild(node);
    addChild(transform);

    m_transformBins[nodeBin].push_back(transform);
  }

  void CountryPresenter::addTransformFromNodeToCenter(osg::Node* node, const std::string& nodeBin,
                                                      const osg::Vec2f& relPosition)
  {
    auto offsetLong = m_size.x() * (relPosition.x() - 0.5f) * 2.0f * C_PI;
    auto offsetLat  = m_size.y() * (relPosition.y() - 0.5f) * C_PI;

    addTransformFromNodeToCoords(
            node, nodeBin,
            osg::Vec2f(offsetLat - m_centerLatLong.x(), fmodf(m_centerLatLong.y() + C_PI + offsetLong, C_PI * 2.0f)));
  }
}
