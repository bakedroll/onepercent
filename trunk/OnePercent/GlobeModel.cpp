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

using namespace onep;
using namespace osg;
using namespace osgGaming;
using namespace std;

const double GlobeModel::EARTH_RADIUS = 6.371;
const double GlobeModel::ATMOSPHERE_HEIGHT = 0.06;
const double GlobeModel::SCATTERING_DEPTH = 0.25;
const double GlobeModel::SCATTERING_INTENSITY = 0.8;
const Vec4f GlobeModel::ATMOSPHERE_COLOR = Vec4f(0.1981f, 0.4656f, 0.8625f, 0.75f);
const int GlobeModel::SPHERE_STACKS = 96;
const int GlobeModel::SPHERE_SLICES = 192;
const double GlobeModel::SUN_DISTANCE = 149600.0;
const double GlobeModel::SUN_RADIUS_PM2 = pow(695.8f, -2.0f);

GlobeModel::GlobeModel()
{
	makeEarthModel();
	makeCloudsModel();
	makeAtmosphericScattering();
}

void GlobeModel::updateScatteringGeometry(ref_ptr<TransformableCameraManipulator> cameraManipulator)
{
	ref_ptr<Vec3Array> verts = static_cast<Vec3Array*>(_scatteringGeometry->getVertexArray());
	ref_ptr<Vec3Array> normals = static_cast<Vec3Array*>(_scatteringGeometry->getNormalArray());

	Vec3f v[4];
	v[0] = Vec3f(-1.0f, -1.0f, -1.0f);
	v[1] = Vec3f(-1.0f, 1.0f, -1.0f);
	v[2] = Vec3f(1.0f, 1.0f, -1.0f);
	v[3] = Vec3f(1.0f, - 1.0f, -1.0f);

	Vec3f normal(0.0f, 0.0f, 1.0f);

	Matrixd mat = Matrix::inverse(cameraManipulator->getViewMatrix() * cameraManipulator->getProjectionMatrix());

	for (int i = 0; i < 4; i++)
	{
		Vec3f v_res = v[i] * mat;
		verts->at(i).set(v_res);

		Vec3f n = ((v[i] + normal) * mat) - v_res;
		n.normalize();
		normals->at(i).set(n);
	}

	normals->dirty();
	verts->dirty();
	_scatteringGeometry->dirtyBound();
}

void GlobeModel::updateLightDirection(osg::Vec3f direction)
{
	_scatteringLightDirUniform->setElement(0, direction);
	Vec3f position = -direction * SUN_DISTANCE;

	_scatteringLightPosrUniform->setElement(0, Vec4f(position.x(), position.y(), position.z(), SUN_RADIUS_PM2));
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

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./shader/globe.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./shader/globe.frag", Shader::FRAGMENT);

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
	ref_ptr<Geode> atmosphere_geode = new Geode();
	ref_ptr<Geometry> atmosphere = createSphereSegmentMesh(SPHERE_STACKS, SPHERE_SLICES, EARTH_RADIUS + ATMOSPHERE_HEIGHT, 0, SPHERE_STACKS - 1, 0, SPHERE_SLICES - 1);

	ref_ptr<Material> material = new Material();
	material->setAlpha(Material::FRONT_AND_BACK, 0.8f);

	atmosphere->getOrCreateStateSet()->setMode(GL_BLEND, StateAttribute::ON);
	atmosphere->getStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
	atmosphere->getStateSet()->setAttribute(material);

	atmosphere_geode->addDrawable(atmosphere);

	//addChild(atmosphere_geode);
}

void GlobeModel::makeAtmosphericScattering()
{
	// scattering stateset
	ref_ptr<StateSet> stateSet = new StateSet();

	stateSet->setMode(GL_BLEND, StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	stateSet->setMode(GL_CULL_FACE, StateAttribute::OFF);
	stateSet->setAttributeAndModes(new BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), StateAttribute::ON);

	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(10, "RenderBin");

	// scattering geometry
	ref_ptr<Geode> geode = new Geode();

	_scatteringGeometry = new Geometry();
	_scatteringGeometry->setUseVertexBufferObjects(true);

	ref_ptr<Vec3Array> verts = new Vec3Array(4);
	verts->setDataVariance(DYNAMIC);
	_scatteringGeometry->setVertexArray(verts);

	ref_ptr<Vec3Array> normals = new Vec3Array(4);
	normals->setDataVariance(DYNAMIC);
	_scatteringGeometry->setNormalArray(normals);
	_scatteringGeometry->setNormalBinding(Geometry::BIND_PER_VERTEX);

	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(Vec4(-1.0f, -1.0f, 0.0f, 1.0f));
	colors->push_back(Vec4(-1.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(Vec4(1.0f, -1.0f, 0.0f, 1.0f));
	_scatteringGeometry->setColorArray(colors);
	_scatteringGeometry->setColorBinding(Geometry::BIND_PER_VERTEX);

	ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(PrimitiveSet::POLYGON, 0);
	indices->push_back(3);
	indices->push_back(2);
	indices->push_back(1);
	indices->push_back(0);
	_scatteringGeometry->addPrimitiveSet(indices);

	geode->addDrawable(_scatteringGeometry);

	// atmospheric scattering shader
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./shader/atmosphere.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./shader/atmosphere.frag", Shader::FRAGMENT);

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

	geode->setStateSet(stateSet);

	addChild(geode);
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

			sprintf(colormap_file, "./data/earth/color/%s/%dx%d.png", resolutionLevel0, x, y);
			sprintf(nightmap_file, "./data/earth/night/%s/%dx%d.png", resolutionLevel0, x, y);
			sprintf(specreliefcitiesboundariesmap_file, "./data/earth/specular_relief_cities/%s/%dx%d.png", resolutionLevel1, x, y);
			sprintf(normalmap_file, "./data/earth/normal/%s/%dx%d.png", resolutionLevel1, x, y);

			loadTexture(stateSet, colormap_file, 0, "colormap");
			loadTexture(stateSet, nightmap_file, 1, "nightmap");
			loadTexture(stateSet, specreliefcitiesboundariesmap_file, 2, "specreliefcitiesboundariesmap");
			loadTexture(stateSet, normalmap_file, 3, "normalmap");

			geode->addDrawable(geo);
		}
	}

	return geode;
}

void GlobeModel::loadTexture(ref_ptr<StateSet> stateSet, string filename, int tex_layer, string uniform_name)
{
	ref_ptr<Texture2D> texture = new Texture2D();
	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setWrap(Texture::WRAP_S, Texture::CLAMP_TO_EDGE);
	texture->setWrap(Texture::WRAP_T, Texture::CLAMP_TO_EDGE);
	texture->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
	texture->setMaxAnisotropy(8);

	ref_ptr<Image> image = ResourceManager::getInstance()->loadImage(filename);
	if (!image.valid())
	{
		return;
	}

	texture->setImage(image);

	ref_ptr<Uniform> uniform = new Uniform(Uniform::SAMPLER_2D, uniform_name);
	uniform->set(tex_layer);

	stateSet->setTextureAttributeAndModes(tex_layer, texture, StateAttribute::ON);
	stateSet->addUniform(uniform);
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