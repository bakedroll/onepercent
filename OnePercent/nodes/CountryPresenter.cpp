#include "nodes/CountryPresenter.h"
#include "core/Macros.h"
#include "data/CountriesMap.h"

#include <osgHelper/Macros.h>


#include <cmath>

#include <QString>

#include <osg/MatrixTransform>

#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>

#include <osgHelper/Helper.h>
#include <osgHelper/Macros.h>

namespace onep
{
float calcSegmentArea(float radius, float r1, float r2)
{
  const auto radius2 = radius * radius;
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
          .addFunction("make_boundaries", &CountryPresenter::luaMakeBoundaries)
          .addFunction("set_boundaries_enabled", &CountryPresenter::luaSetBoundariesEnabled)
          .addFunction("add_node", &CountryPresenter::luaAddNode)
          .addFunction("add_node_to_bin", &CountryPresenter::luaAddNodeToBin)
          .addFunction("add_node_to_bin_at", &CountryPresenter::luaAddNodeToBinAt)
          .addFunction("scatter_nodes_to_bin", &CountryPresenter::luaScatterNodesToBin)
          .addFunction("remove_node_bin", &CountryPresenter::luaRemoveNodeBin)
          .addFunction("clear_nodes", &CountryPresenter::luaClearNodes)
          .endClass();
}

CountryPresenter::CountryPresenter(int id, const LuaConfig::Ptr& configManager,
                                   const std::shared_ptr<CountriesMap>& countriesMap,
                                   const osg::ref_ptr<BoundariesData>& boundariesData, const osg::Vec2f& centerLatLong,
                                   const osg::Vec2f& size, const BoundariesData::BorderIdMap& neighbourBorders)
  : osg::Switch(),
    m_id(id),
    m_centerLatLong(centerLatLong),
    m_size(size),
    m_earthRadius(configManager->getNumber<float>("earth.radius")),
    m_cameraZoom(configManager->getNumber<float>("camera.country_zoom")),
    m_countriesMap(countriesMap),
    m_boundariesData(boundariesData),
    m_neighbourBorders(neighbourBorders)
{
}

CountryPresenter::~CountryPresenter()
{
  m_countriesMap.reset();
  m_boundariesData.release();
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
  return osg::Vec2f(2.0f * C_PI * sin(C_PI / 2.0f - abs(m_centerLatLong.x())) * m_earthRadius * m_size.x(),
                    C_PI * m_earthRadius * m_size.y());
}

float CountryPresenter::getOptimalCameraDistance(float angle, float ratio) const
{
  const auto surfaceSize = getSurfaceSize();

  const auto hdistance = surfaceSize.x() * m_cameraZoom / (2.0f * tan(angle * ratio * C_PI / 360.0f));
  const auto vdistance = surfaceSize.y() * m_cameraZoom / (2.0f * tan(angle * C_PI / 360.0f));

  return std::max(hdistance, vdistance);
}

const BoundariesData::BorderIdMap& CountryPresenter::getNeighborBorders() const
{
  return m_neighbourBorders;
}

bool CountryPresenter::getIsOnOcean() const
{
  return m_neighbourBorders.find(-1) != m_neighbourBorders.end();
}

void CountryPresenter::luaMakeBoundaries(const osg::Vec3f& color, float thickness)
{
  if (!m_boundsMesh.valid())
  {
    m_boundsMesh = new osg::Geode();

    auto stateSet = m_boundsMesh->getOrCreateStateSet();
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateSet->setRenderBinDetails(10, "RenderBin");

    addChild(m_boundsMesh, true);
  }

  if (m_boundsMesh->getNumDrawables() > 0)
  {
    m_boundsMesh->removeDrawables(0);
  }

  BoundariesData::CountryBorders borders;
  borders[m_id] = m_neighbourBorders;

  m_boundsMesh->addDrawable(m_boundariesData->createCountryBoundariesGeometry(borders, color, thickness));
}

void CountryPresenter::luaSetBoundariesEnabled(bool enabled)
{
  if (!m_boundsMesh.valid())
  {
    OSGH_QLOG_WARN(QString("Country with id '%1' has no boundaries").arg(m_id));
    assert_return(false);
  }

  setChildValue(m_boundsMesh, enabled);
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
    const auto latitude  = latMin + rand01() * latCoverage;
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
    OSGH_QLOG_WARN(QString("Node bin '%1' not found").arg(nodeBin.c_str()));
    return;
  }

  auto it = m_transformBins.find(nodeBin);
  for (const auto& transform : it->second)
  {
    getOrCreateNodesGroup()->removeChild(transform);
  }
  m_transformBins.erase(it);
}

void CountryPresenter::luaClearNodes()
{
  m_transformBins.clear();

  const auto& nodesGroup = getOrCreateNodesGroup();
  nodesGroup->removeChildren(0, nodesGroup->getNumChildren());
}

osg::ref_ptr<osg::Group>& CountryPresenter::getOrCreateNodesGroup()
{
  if (!m_nodes.valid())
  {
    m_nodes = new osg::Group();
    m_nodes->getOrCreateStateSet()->setRenderBinDetails(20, "RenderBin");
    addChild(m_nodes, true);
  }

  return m_nodes;
}

float CountryPresenter::getSurfaceArea() const
{
  const auto alpha  = m_centerLatLong.x();
  const auto height = m_size.y() * C_PI * 0.5f;

  const auto a1 = alpha + height;
  const auto a2 = alpha - height;
  auto       r1 = m_earthRadius * cos(a1);
  auto       r2 = m_earthRadius * cos(a2);

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
  const auto matRot  = osg::Matrix::rotate(osgHelper::getQuatFromEuler(latLong.x(), 0.0f, latLong.y()));
  const auto matMove = osg::Matrix::translate(osg::Vec3f(0.0f, -m_earthRadius, 0.0f));

  osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
  transform->setMatrix(matMove * matRot);

  transform->addChild(node);
  getOrCreateNodesGroup()->addChild(transform);

  m_transformBins[nodeBin].push_back(transform);
}

void CountryPresenter::addTransformFromNodeToCenter(osg::Node* node, const std::string& nodeBin,
                                                    const osg::Vec2f& relPosition)
{
  const auto offsetLong = m_size.x() * (relPosition.x() - 0.5f) * 2.0f * C_PI;
  const auto offsetLat  = m_size.y() * (relPosition.y() - 0.5f) * C_PI;

  addTransformFromNodeToCoords(
          node, nodeBin,
          osg::Vec2f(offsetLat - m_centerLatLong.x(), fmodf(m_centerLatLong.y() + C_PI + offsetLong, C_PI * 2.0f)));
}
}  // namespace onep
