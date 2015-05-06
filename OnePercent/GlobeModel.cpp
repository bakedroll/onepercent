#include "GlobeModel.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Point>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/Helper.h>
#include <osgGaming/CameraAlignedQuad.h>
#include <osgGaming/TextureFactory.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;
using namespace std;

const double GlobeModel::EARTH_RADIUS = 6.371;
const double GlobeModel::CLOUDS_HEIGHT = 0.004;
const double GlobeModel::ATMOSPHERE_HEIGHT = 0.06;
const double GlobeModel::SCATTERING_DEPTH = 0.25;
const double GlobeModel::SCATTERING_INTENSITY = 0.8;
const Vec4f GlobeModel::ATMOSPHERE_COLOR = Vec4f(0.1981f, 0.4656f, 0.8625f, 0.75f);
const int GlobeModel::SPHERE_STACKS = 96;
const int GlobeModel::SPHERE_SLICES = 192;
const double GlobeModel::SUN_DISTANCE = 149600.0;
const double GlobeModel::SUN_RADIUS_PM2 = pow(695.8f, -2.0f);

GlobeModel::GlobeModel(osg::ref_ptr<TransformableCameraManipulator> tcm)
{
	makeEarthModel();
	makeCloudsModel();
	makeAtmosphericScattering(tcm);
}

void GlobeModel::updateLightDirection(osg::Vec3f direction)
{
	_scatteringLightDirUniform->setElement(0, direction);
	Vec3f position = -direction * SUN_DISTANCE;

	_scatteringLightPosrUniform->setElement(0, Vec4f(position.x(), position.y(), position.z(), SUN_RADIUS_PM2));
}

void GlobeModel::updateClouds(double simTime)
{
	Quat quat = getQuatFromEuler(0.0, 0.0, fmodf(simTime * 0.003f, 2.0f * C_PI));
	_cloudsTransform->setAttitude(quat);
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

	earth->setStateSet(stateSet);

	addChild(earth);
}

void GlobeModel::makeCloudsModel()
{
	// geometry
	_cloudsTransform = new PositionAttitudeTransform();

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
	stateSet->setRenderingHint(StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(0, "RenderBin");
	stateSet->setAttribute(material);

	// shader
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/clouds.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/clouds.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	stateSet->setAttribute(pgm, StateAttribute::ON);

	atmosphere_geode->setStateSet(stateSet);

	_cloudsTransform->addChild(atmosphere_geode);
	addChild(_cloudsTransform);
}

void GlobeModel::makeAtmosphericScattering(osg::ref_ptr<TransformableCameraManipulator> tcm)
{
	// atmospheric scattering geometry
	ref_ptr<CameraAlignedQuad> caq = new CameraAlignedQuad();

	// shader
	ref_ptr<StateSet> stateSet = caq->getOrCreateStateSet();

	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/atmosphere.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/atmosphere.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	double earth_rad = pow(EARTH_RADIUS * 0.9999, -2.0f);
	double atmos_rad = pow(EARTH_RADIUS + ATMOSPHERE_HEIGHT, -2.0f);

	stateSet->addUniform(new Uniform("planet_r", Vec3f(earth_rad, earth_rad, earth_rad)));
	stateSet->addUniform(new Uniform("planet_R", Vec3f(atmos_rad, atmos_rad, atmos_rad)));
	stateSet->addUniform(new Uniform("planet_h", (float)ATMOSPHERE_HEIGHT));
	stateSet->addUniform(new Uniform("view_depth", (float)SCATTERING_DEPTH));

	_scatteringLightDirUniform = new Uniform(Uniform::FLOAT_VEC3, "light_dir", 1);
	ref_ptr<Uniform> light_col_uniform = new Uniform(Uniform::FLOAT_VEC3, "light_col", 1);
	_scatteringLightPosrUniform = new Uniform(Uniform::FLOAT_VEC4, "light_posr", 1);

	//light_col_uniform->setElement(0, Vec3f(1.0f, 1.0f, 1.0f));
	light_col_uniform->setElement(0, Vec3f(0.0f, 0.0f, 0.0f));

	stateSet->addUniform(new Uniform("lights", 1));
	stateSet->addUniform(_scatteringLightDirUniform);
	stateSet->addUniform(light_col_uniform);
	stateSet->addUniform(_scatteringLightPosrUniform);

	stateSet->addUniform(new Uniform("B0", ATMOSPHERE_COLOR * SCATTERING_INTENSITY));

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

	int stacksPerSegment = SPHERE_STACKS / m;
	int slicesPerSegment = SPHERE_SLICES / n;

	for (int y = 0; y < m; y++)
	{
		for (int x = 0; x < n; x++)
		{
			ref_ptr<Geometry> geo = createSphereSegmentMesh(
				SPHERE_STACKS, SPHERE_SLICES, EARTH_RADIUS,
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
			sprintf(specreliefcitiesboundariesmap_file, "./GameData/textures/earth/specular_relief_cities/%s/%dx%d.png", resolutionLevel1, x, y);
			sprintf(normalmap_file, "./GameData/textures/earth/normal/%s/%dx%d.png", resolutionLevel1, x, y);

			TextureFactory::make()
				->image(ResourceManager::getInstance()->loadImage(colormap_file))
				->texLayer(0)
				->uniform(stateSet, "colormap")
				->assign(stateSet)
				->build();

			TextureFactory::make()
				->image(ResourceManager::getInstance()->loadImage(nightmap_file))
				->texLayer(1)
				->uniform(stateSet, "nightmap")
				->assign(stateSet)
				->build();

			TextureFactory::make()
				->image(ResourceManager::getInstance()->loadImage(specreliefcitiesboundariesmap_file))
				->texLayer(2)
				->uniform(stateSet, "specreliefcitiesboundariesmap")
				->assign(stateSet)
				->build();

			TextureFactory::make()
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

	int stacksPerSegment = SPHERE_STACKS / m;
	int slicesPerSegment = SPHERE_SLICES / n;

	for (int y = 0; y < m; y++)
	{
		for (int x = 0; x < n; x++)
		{
			ref_ptr<Geometry> geo = createSphereSegmentMesh(
				SPHERE_STACKS, SPHERE_SLICES, EARTH_RADIUS + CLOUDS_HEIGHT,
				y * stacksPerSegment,
				(y + 1) * stacksPerSegment - 1,
				x * slicesPerSegment,
				(x + 1) * slicesPerSegment - 1);

			ref_ptr<StateSet> stateSet = geo->getOrCreateStateSet();

			char colormap_file[128];
			sprintf(colormap_file, "./GameData/textures/earth/clouds/8k/%dx%d.png", x, y);

			TextureFactory::make()
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
			Vec3 point = getVec3FromEuler((double)stack * (C_PI / (double)stacks), 0.0, (double)slice * (2.0 * C_PI / (double)slices), Vec3(0.0, 0.0, 1.0));

			vertices->push_back(point * radius);
			normals->push_back(point);

			double u = (double)(slice - firstSlice) / (double)(lastSlice - firstSlice + 1);
			double v = (double)(stack - firstStack) / (double)(lastStack - firstStack + 1);

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