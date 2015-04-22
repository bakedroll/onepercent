#include "GlobeModel.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Point>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;
using namespace std;

const double GlobeModel::EARTH_RADIUS = 6.371;
const int GlobeModel::SPHERE_STACKS = 48;
const int GlobeModel::SPHERE_SLICES = 96;

GlobeModel::GlobeModel()
{
	ref_ptr<Node> earth = createPlanetGeode(0);
	//ref_ptr<Node> atmosphere = createMesh(48, 96, 6385);

	ref_ptr<StateSet> ss = createStateSet();
	ss->setAttribute(createShader());

	earth->setStateSet(ss);
	generateTangentAndBinormal(earth);


	//ref_ptr<Material> material = new Material();
	//material->setAlpha(Material::FRONT_AND_BACK, 0.8f);


	//atmosphere->getOrCreateStateSet()->setMode(GL_BLEND, StateAttribute::ON);
	//atmosphere->getStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
	//atmosphere->getStateSet()->setAttribute(material);


	addChild(earth);
	// addChild(atmosphere);
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

ref_ptr<StateSet> GlobeModel::createStateSet()
{
	ref_ptr<StateSet> stateSet = new StateSet();

	ref_ptr<Material> material = new Material();
	material->setAmbient(Material::FRONT_AND_BACK, Vec4f(0.1, 0.1, 0.1, 1.0));
	material->setDiffuse(Material::FRONT_AND_BACK, Vec4f(1.0, 1.0, 1.0, 1.0));
	material->setSpecular(Material::FRONT_AND_BACK, Vec4f(0.5, 0.5, 0.5, 1.0));
	material->setShininess(Material::FRONT_AND_BACK, 16);
	material->setEmission(Material::FRONT_AND_BACK, Vec4f(0.0, 0.0, 0.0, 1.0));

	stateSet->setAttribute(material);

	return stateSet;
}

ref_ptr<Program> GlobeModel::createShader()
{
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = new Shader(Shader::VERTEX);
	ref_ptr<Shader> frag_shader = new Shader(Shader::FRAGMENT);

	vert_shader->loadShaderSourceFromFile("./shader/globe.vert");
	frag_shader->loadShaderSourceFromFile("./shader/globe.frag");

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	pgm->addBindAttribLocation("tangent", 6);
	pgm->addBindAttribLocation("binormal", 7);

	return pgm;
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

	ref_ptr<Image> image = osgDB::readImageFile(filename);
	if (!image)
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