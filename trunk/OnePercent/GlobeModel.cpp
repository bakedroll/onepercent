#include "GlobeModel.h"

#include <osg/Geode>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>
#include <osgGaming/CameraAlignedQuad.h>
#include <osgGaming/TextureFactory.h>
#include <osgGaming/ByteStream.h>
#include <osg/Switch>

using namespace onep;
using namespace osg;
using namespace osgGaming;
using namespace std;

GlobeModel::GlobeModel(osg::ref_ptr<TransformableCameraManipulator> tcm)
{
	makeEarthModel();
	makeCloudsModel();
  makeBoundariesModel();
	makeAtmosphericScattering(tcm);
}

void GlobeModel::updateLightDirection(osg::Vec3f direction)
{
	m_scatteringLightDirUniform->setElement(0, direction);
	Vec3f position = -direction * ~m_paramSunDistance;

	m_scatteringLightPosrUniform->setElement(0, Vec4f(position.x(), position.y(), position.z(), ~m_paramSunRadiusMp2));
}

void GlobeModel::updateClouds(float day)
{
	Quat quat = getQuatFromEuler(0.0, 0.0, fmodf(day * ~m_paramEarthCloudsSpeed, C_2PI));
	m_cloudsTransform->setAttitude(quat);

	m_uniformTime->set(day * ~m_paramEarthCloudsMorphSpeed);
}

void GlobeModel::setSelectedCountry(int countryId)
{
  for (CountryMesh::List::iterator it = m_visibleCountryMeshs.begin(); it != m_visibleCountryMeshs.end(); ++it)
    m_countrySurfacesSwitch->setChildValue(*it, false);

  if (countryId == 0)
    return;

  CountryMesh::Ptr mesh = m_countryMeshs.find(countryId)->second;
  mesh->setColorMode(CountryMesh::MODE_SELECTED);
  m_countrySurfacesSwitch->setChildValue(mesh, true);
  m_visibleCountryMeshs.push_back(mesh);

  CountryMesh::Neighbor::List& neighbors = mesh->getNeighborCountryMeshs();
  for (CountryMesh::Neighbor::List::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
  {
    it->mesh->setColorMode(CountryMesh::MODE_NEIGHBOR);
    m_countrySurfacesSwitch->setChildValue(it->mesh, true);
    m_visibleCountryMeshs.push_back(it->mesh);
  }
}

void GlobeModel::setCountriesMap(CountriesMap::Ptr countriesMap)
{
  m_countriesMap = countriesMap;
}

void GlobeModel::addCountry(int id, Country::Ptr countryData, osg::ref_ptr<osg::DrawElementsUInt> triangles)
{
  if (m_countryMeshs.find(id) != m_countryMeshs.end())
    return;

  if (!m_countrySurfacesSwitch.valid())
  {
    m_countrySurfacesSwitch = new Switch();
    m_countrySurfacesSwitch->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF);
    m_countrySurfacesSwitch->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
    m_countrySurfacesSwitch->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    m_countrySurfacesSwitch->getOrCreateStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
    m_countrySurfacesSwitch->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");

    addChild(m_countrySurfacesSwitch);
  }

  CountryMesh::Ptr mesh = new CountryMesh(m_countriesVertices, triangles);
  mesh->setCountryData(countryData);

  m_countryMeshs.insert(CountryMesh::Map::value_type(id, mesh));
  m_countrySurfacesSwitch->addChild(mesh, false);
}

CountryMesh::Map& GlobeModel::getCountryMeshs()
{
  return m_countryMeshs;
}

CountriesMap::Ptr GlobeModel::getCountriesMap()
{
  return m_countriesMap;
}

CountryMesh::Ptr GlobeModel::getCountryMesh(int id)
{
  CountryMesh::Map::iterator countryMesh = m_countryMeshs.find(id);

  if (countryMesh == m_countryMeshs.end())
    return nullptr;

  return m_countryMeshs.find(id)->second;
}

CountryMesh::Ptr GlobeModel::getCountryMesh(osg::Vec2f coord)
{
  int id = getCountryId(coord);

  if (id == 0)
    return nullptr;

  return getCountryMesh(id);
}

int GlobeModel::getCountryId(osg::Vec2f coord)
{
  Vec2i mapSize = m_countriesMap->getSize();

  int ix = int(coord.x() * float(mapSize.x()));
  int iy = int(coord.y() * float(mapSize.y()));

  return m_countriesMap->getDataAt(ix, iy);
}

std::string GlobeModel::getCountryName(osg::Vec2f coord)
{
  CountryMesh::Map::iterator it = m_countryMeshs.find(getCountryId(coord));
  if (it == m_countryMeshs.end())
  {
    return "No country selected";
  }

  return it->second->getCountryData()->getCountryName();
}

void GlobeModel::makeEarthModel()
{
	// planet geometry
	ref_ptr<Node> earth = createPlanetGeode(0);
	generateTangentAndBinormal(earth);

	// stateset
	ref_ptr<StateSet> stateSet = new StateSet();

	ref_ptr<Material> material = new Material();
	material->setAmbient(Material::FRONT_AND_BACK, Vec4f(0.1, 0.1, 0.1, 1.0));
	material->setDiffuse(Material::FRONT_AND_BACK, Vec4f(1.0, 1.0, 1.0, 1.0));
	material->setSpecular(Material::FRONT_AND_BACK, Vec4f(0.5, 0.5, 0.5, 1.0));
	material->setShininess(Material::FRONT_AND_BACK, 16);
	material->setEmission(Material::FRONT_AND_BACK, Vec4f(0.0, 0.0, 0.0, 1.0));

	stateSet->setAttribute(material);

	// shader
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/globe.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/globe.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	pgm->addBindAttribLocation("tangent", 6);
	pgm->addBindAttribLocation("binormal", 7);

	stateSet->setAttribute(pgm, StateAttribute::ON);
  //stateSet->setRenderBinDetails(0, "RenderBin");

	earth->setStateSet(stateSet);

	addChild(earth);
}

void GlobeModel::makeCloudsModel()
{
	// geometry
	m_cloudsTransform = new PositionAttitudeTransform();

	ref_ptr<Geode> atmosphere_geode = createCloudsGeode();

	// stateset
	ref_ptr<StateSet> stateSet = new StateSet();

	ref_ptr<Material> material = new Material();
	material->setAmbient(Material::FRONT_AND_BACK, Vec4f(0.2, 0.2, 0.2, 1.0));
	material->setDiffuse(Material::FRONT_AND_BACK, Vec4f(1.0, 1.0, 1.0, 1.0));
	material->setSpecular(Material::FRONT_AND_BACK, Vec4f(0.2, 0.2, 0.2, 1.0));
	material->setShininess(Material::FRONT_AND_BACK, 32);
	material->setEmission(Material::FRONT_AND_BACK, Vec4f(0.0, 0.0, 0.0, 1.0));

	stateSet->setMode(GL_BLEND, StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setRenderingHint(StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(0, "RenderBin");
	stateSet->setAttribute(material);

	// shader
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/clouds.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/clouds.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	m_uniformTime = new Uniform("time", 0.0f);

	stateSet->setAttribute(pgm, StateAttribute::ON);
	stateSet->addUniform(m_uniformTime);

	atmosphere_geode->setStateSet(stateSet);

	m_cloudsTransform->addChild(atmosphere_geode);
	addChild(m_cloudsTransform);
}

void GlobeModel::makeBoundariesModel()
{
  char* bytes = ResourceManager::getInstance()->loadBinary("./GameData/data/boundaries.dat");
  ByteStream stream(bytes);

  ref_ptr<Geode> geode = new Geode();

  m_countriesVertices = new Vec3Array();

  int nverts = stream.read<int>();
  for (int i = 0; i < nverts; i++)
  {
    float x = stream.read<float>();
    float y = stream.read<float>();
    float z = stream.read<float>();

    m_countriesVertices->push_back(Vec3f(x, y, z));
  }

  int nInnerPoints = stream.read<int>();
  ref_ptr<Vec2Array> texcoords = new Vec2Array();
  for (int i = 0; i < nInnerPoints; i++)
    texcoords->push_back(Vec2f(1.0f, 0.0f));

  for (int i = 0; i < (nverts - nInnerPoints); i++)
    texcoords->push_back(Vec2f(0.0f, 0.0f));

  /*int nedges = stream.read<int>();
  for (int i = 0; i < nedges; i++)
  {
    stream.read<int>();
    stream.read<int>();
  }*/

  ref_ptr<DrawElementsUInt> quads = new DrawElementsUInt(GL_QUADS, 0);

  int nquads = stream.read<int>();
  for (int i = 0; i < nquads; i++)
  {
    for (int j = 0; j < 4; j++)
      quads->push_back(stream.read<int>());
  }

  ref_ptr<Geometry> geo_quads = new Geometry();
  geo_quads->setVertexArray(m_countriesVertices);
  geo_quads->setTexCoordArray(0, texcoords, Array::BIND_PER_VERTEX);
  geo_quads->addPrimitiveSet(quads);

  geode->addDrawable(geo_quads);
  addChild(geode);

  ref_ptr<Program> program = new Program();
  ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/boundaries.frag", Shader::FRAGMENT);

  program->addShader(frag_shader);

  geode->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF);
  geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
  geode->getOrCreateStateSet()->setAttribute(program, StateAttribute::ON);
  geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
  geode->getOrCreateStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
  geode->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

  ResourceManager::getInstance()->clearCacheResource("./GameData/data/boundaries.dat");
}

void GlobeModel::makeAtmosphericScattering(osg::ref_ptr<TransformableCameraManipulator> tcm)
{
	float earthRadius = ~Property<float, Param_EarthRadiusName>();
	float atmosphereHeight = ~Property<float, Param_EarthAtmosphereHeightName>();
	float scatteringDepth = ~Property<float, Param_EarthScatteringDepthName>();
	float scatteringIntensity = ~Property<float, Param_EarthScatteringIntensityName>();
	Vec4f atmosphereColor = ~Property<Vec4f, Param_EarthAtmosphereColorName>();

	// atmospheric scattering geometry
	ref_ptr<CameraAlignedQuad> caq = new CameraAlignedQuad();

	// shader
	ref_ptr<StateSet> stateSet = caq->getOrCreateStateSet();
	stateSet->setAttributeAndModes(new BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), StateAttribute::ON);

	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/atmosphere.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/atmosphere.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	double earth_rad = pow(earthRadius * 0.9999, -2.0f);
	double atmos_rad = pow(earthRadius + atmosphereHeight, -2.0f);

	stateSet->addUniform(new Uniform("planet_r", Vec3f(earth_rad, earth_rad, earth_rad)));
	stateSet->addUniform(new Uniform("planet_R", Vec3f(atmos_rad, atmos_rad, atmos_rad)));
	stateSet->addUniform(new Uniform("planet_h", atmosphereHeight));
	stateSet->addUniform(new Uniform("view_depth", scatteringDepth));

	m_scatteringLightDirUniform = new Uniform(Uniform::FLOAT_VEC3, "light_dir", 1);
	ref_ptr<Uniform> light_col_uniform = new Uniform(Uniform::FLOAT_VEC3, "light_col", 1);
	m_scatteringLightPosrUniform = new Uniform(Uniform::FLOAT_VEC4, "light_posr", 1);

	//light_col_uniform->setElement(0, Vec3f(1.0f, 1.0f, 1.0f));
	light_col_uniform->setElement(0, Vec3f(0.0f, 0.0f, 0.0f));

	stateSet->addUniform(new Uniform("lights", 1));
	stateSet->addUniform(m_scatteringLightDirUniform);
	stateSet->addUniform(light_col_uniform);
	stateSet->addUniform(m_scatteringLightPosrUniform);

	stateSet->addUniform(new Uniform("B0", atmosphereColor * scatteringIntensity));

	stateSet->setAttribute(pgm, StateAttribute::ON);

	addChild(caq);
	tcm->addCameraAlignedQuad(caq);
}

ref_ptr<Geode> GlobeModel::createPlanetGeode(int textureResolution)
{
	ref_ptr<Geode> geode = new Geode();

	char* resolutionLevel0;
	char* resolutionLevel1;
	int n, m;

	switch (textureResolution)
	{
	default:
	case 0:
		n = 2;
		m = 1;
		resolutionLevel0 = "8k";
		resolutionLevel1 = "8k";
		break;
	case 1:
		n = 4;
		m = 2;
		resolutionLevel0 = "16k";
		resolutionLevel1 = "8k_4x2";
		break;
	};

	int stacks = ~Property<int, Param_EarthSphereStacksName>();
	int slices = ~Property<int, Param_EarthSphereSlicesName>();
	float radius = ~Property<float, Param_EarthRadiusName>();

	int stacksPerSegment = stacks / m;
	int slicesPerSegment = slices / n;

	for (int y = 0; y < m; y++)
	{
		for (int x = 0; x < n; x++)
		{
			ref_ptr<Geometry> geo = createSphereSegmentMesh(
				stacks, slices, radius,
				y * stacksPerSegment,
				(y + 1) * stacksPerSegment - 1,
				x * slicesPerSegment,
				(x + 1) * slicesPerSegment - 1);

			ref_ptr<StateSet> stateSet = geo->getOrCreateStateSet();

			char colormap_file[128];
			char nightmap_file[128];
			char specreliefcitiesboundariesmap_file[128];
			char normalmap_file[128];

			sprintf(colormap_file, "./GameData/textures/earth/color/%s/%dx%d.png", resolutionLevel0, x, y);
			sprintf(nightmap_file, "./GameData/textures/earth/night/%s/%dx%d.png", resolutionLevel0, x, y);
			sprintf(specreliefcitiesboundariesmap_file, "./GameData/textures/earth/speccitiesclouds/%s/%dx%d.png", resolutionLevel1, x, y);
			sprintf(normalmap_file, "./GameData/textures/earth/normal/%s/%dx%d.png", resolutionLevel1, x, y);

			TextureFactory::getInstance()->make()
				->image(ResourceManager::getInstance()->loadImage(colormap_file))
				->texLayer(0)
				->uniform(stateSet, "colormap")
				->assign(stateSet)
				->build();

			TextureFactory::getInstance()->make()
				->image(ResourceManager::getInstance()->loadImage(nightmap_file))
				->texLayer(1)
				->uniform(stateSet, "nightmap")
				->assign(stateSet)
				->build();

			TextureFactory::getInstance()->make()
				->image(ResourceManager::getInstance()->loadImage(specreliefcitiesboundariesmap_file))
				->texLayer(2)
				->uniform(stateSet, "speccitiescloudsmap")
				->assign(stateSet)
				->build();

			TextureFactory::getInstance()->make()
				->image(ResourceManager::getInstance()->loadImage(normalmap_file))
				->texLayer(3)
				->uniform(stateSet, "normalmap")
				->assign(stateSet)
				->build();

			geode->addDrawable(geo);
		}
	}

	return geode;
}

ref_ptr<Geode> GlobeModel::createCloudsGeode()
{
	ref_ptr<Geode> geode = new Geode();

	int n = 2;
	int m = 1;

	int stacks = ~Property<int, Param_EarthSphereStacksName>();
	int slices = ~Property<int, Param_EarthSphereSlicesName>();
	float radius = ~Property<float, Param_EarthRadiusName>();
	float cloudsHeight = ~Property<float, Param_EarthCloudsHeightName>();

	int stacksPerSegment = stacks / m;
	int slicesPerSegment = slices / n;

	for (int y = 0; y < m; y++)
	{
		for (int x = 0; x < n; x++)
		{
			ref_ptr<Geometry> geo = createSphereSegmentMesh(
				stacks, slices, radius + cloudsHeight,
				y * stacksPerSegment,
				(y + 1) * stacksPerSegment - 1,
				x * slicesPerSegment,
				(x + 1) * slicesPerSegment - 1);

			ref_ptr<StateSet> stateSet = geo->getOrCreateStateSet();

			char colormap_file[128];
			sprintf(colormap_file, "./GameData/textures/earth/speccitiesclouds/8k/%dx%d.png", x, y);

			TextureFactory::getInstance()->make()
				->image(ResourceManager::getInstance()->loadImage(colormap_file))
				->texLayer(0)
				->uniform(stateSet, "colormap")
				->assign(stateSet)
				->build();

			geode->addDrawable(geo);
		}
	}

	return geode;
}

ref_ptr<Geometry> GlobeModel::createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice)
{
	ref_ptr<Geometry> geometry = new Geometry();

	ref_ptr<Vec3Array> vertices = new Vec3Array();
	ref_ptr<Vec3Array> normals = new Vec3Array();
	ref_ptr<Vec2Array> texcoords = new Vec2Array();
	ref_ptr<DrawElementsUInt> triangles = new DrawElementsUInt(PrimitiveSet::TRIANGLES, 0);

	for (int slice = firstSlice; slice <= lastSlice + 1; slice++)
	{
		for (int stack = firstStack; stack <= lastStack + 1; stack++)
		{
			Vec3 point = getVec3FromEuler(double(stack) * (C_PI / double(stacks)), 0.0, double(slice) * (2.0 * C_PI / double(slices)), Vec3(0.0, 0.0, 1.0));

			vertices->push_back(point * radius);
			normals->push_back(point);

			double u = double(slice - firstSlice) / double(lastSlice - firstSlice + 1);
			double v = double(stack - firstStack) / double(lastStack - firstStack + 1);

			texcoords->push_back(Vec2(u, 1.0 - v));
		}
	}

	int nSlices = lastSlice - firstSlice + 1;
	int nStacks = lastStack - firstStack + 1;

	for (int slice = 0; slice < nSlices; slice++)
	{
		int slice_i = (nStacks + 1) * slice;
		int next_slice_i = slice_i + nStacks + 1;

		for (int stack = 0; stack < nStacks; stack++)
		{
			if (stack == 0 && firstStack == 0)
			{
				triangles->push_back(slice_i);
				triangles->push_back(slice_i + 1);
				triangles->push_back(next_slice_i + 1);
			}
			else if (stack == nStacks - 1 && lastStack == stacks - 1)
			{
				triangles->push_back(slice_i + nStacks);
				triangles->push_back(next_slice_i + nStacks - 1);
				triangles->push_back(slice_i + nStacks - 1);
			}
			else
			{
				triangles->push_back(slice_i + stack);
				triangles->push_back(slice_i + stack + 1);
				triangles->push_back(next_slice_i + stack);

				triangles->push_back(next_slice_i + stack);
				triangles->push_back(slice_i + stack + 1);
				triangles->push_back(next_slice_i + stack + 1);
			}
		}
	}
	
	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(Vec4(1.0, 1.0, 1.0, 1.0));

	geometry->setVertexArray(vertices);
	geometry->setNormalArray(normals, Array::BIND_PER_VERTEX);
	geometry->setTexCoordArray(0, texcoords, Array::BIND_PER_VERTEX);
	geometry->setColorArray(colors, Array::BIND_OVERALL);
	geometry->addPrimitiveSet(triangles);

	return geometry;
}