#include "GlobeModel.h"

#include "Helper.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Point>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

using namespace onep;

GlobeModel::GlobeModel()
{
	ref_ptr<Node> earth = createMesh(48, 96, 6371);
	ref_ptr<Node> atmosphere = createMesh(48, 96, 6385);

	ref_ptr<StateSet> ss = createStateSet();
	ss->setAttribute(createShader());

	earth->setStateSet(ss);
	generateTangentAndBinormal(earth);


	ref_ptr<Material> material = new Material();
	material->setAlpha(Material::FRONT_AND_BACK, 0.8f);


	atmosphere->getOrCreateStateSet()->setMode(GL_BLEND, StateAttribute::ON);
	atmosphere->getStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
	atmosphere->getStateSet()->setAttribute(material);


	addChild(earth);
	// addChild(atmosphere);
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

	loadTextures(stateSet, "./data/earth/surface/4_no_ice_clouds_mts_8k.jpg", 0, "colormap");
	loadTextures(stateSet, "./data/earth/surface/5_night_8k.jpg", 1, "nightmap");
	loadTextures(stateSet, "./data/earth/surface/specular_relief_cities_8k.png", 2, "specreliefcitiesmap");
	loadTextures(stateSet, "./data/earth/normal/with_ocean_fixed.jpg", 3, "normalmap");


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

void GlobeModel::loadTextures(ref_ptr<StateSet> stateSet, char* filename, int tex_layer, char* uniform_name)
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

ref_ptr<Geode> GlobeModel::createMesh(int stacks, int slices, double radius)
{
	ref_ptr<Geode> geode = new Geode();
	ref_ptr<Geometry> geometry = new Geometry();

	ref_ptr<Vec3Array> vertices = new Vec3Array();
	ref_ptr<Vec3Array> normals = new Vec3Array();
	ref_ptr<Vec2Array> texcoords = new Vec2Array();
	ref_ptr<DrawElementsUInt> triangles = new DrawElementsUInt(PrimitiveSet::TRIANGLES, 0);

	for (int slice = 0; slice < slices + 1; slice++)
	{
		for (int stack = 0; stack < stacks + 1; stack++)
		{
			Vec3 point = getVec3FromEuler((double)stack * (C_PI / (double)stacks), 0.0, (double)slice * (2.0 * C_PI / (double)slices), Vec3(0.0, 0.0, 1.0));

			vertices->push_back(point * radius);
			normals->push_back(point);
			texcoords->push_back(Vec2((double)slice / (double)slices, 1.0 - (double)stack / (double)stacks));
		}
	}

	for (int slice = 0; slice < slices; slice++)
	{
		int north = ((stacks + 1) * slice);
		int south = north + stacks;
		int slice_i = north + 1;
		int next_slice_i = ((stacks + 1) * (slice + 1)) + 1;

		triangles->push_back(north);
		triangles->push_back(slice_i);
		triangles->push_back(next_slice_i);

		for (int stack = 0; stack < stacks-2; stack++)
		{
			triangles->push_back(slice_i + stack);
			triangles->push_back(slice_i + stack + 1);
			triangles->push_back(next_slice_i + stack);

			triangles->push_back(next_slice_i + stack);
			triangles->push_back(slice_i + stack + 1);
			triangles->push_back(next_slice_i + stack + 1);
		}

		triangles->push_back(south);
		triangles->push_back(next_slice_i + stacks - 2);
		triangles->push_back(slice_i + stacks - 2);
	}
	
	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(Vec4(1.0, 1.0, 1.0, 1.0));

	geometry->setVertexArray(vertices);
	geometry->setNormalArray(normals, Array::BIND_PER_VERTEX);
	geometry->setTexCoordArray(0, texcoords, Array::BIND_PER_VERTEX);
	geometry->setColorArray(colors, Array::BIND_OVERALL);
	geometry->addPrimitiveSet(triangles);

	geode->addDrawable(geometry);

	return geode;
}