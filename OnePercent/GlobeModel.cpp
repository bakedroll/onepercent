#include "GlobeModel.h"

#include "Helper.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Point>
#include <osg/Texture2D>

#include <osgDB/ReadFile>

using namespace onep;

GlobeModel::GlobeModel()
{
	ref_ptr<Node> node = createMesh(24, 48);

	ref_ptr<StateSet> ss = createStateSet();
	//ss->setAttribute(createShader());

	setStateSet(ss);

	generateTangentAndBinormal(node);

	addChild(node);
}

ref_ptr<StateSet> GlobeModel::createStateSet()
{
	ref_ptr<StateSet> stateSet = new StateSet();

	ref_ptr<Material> material = new Material();
	material->setAmbient(Material::FRONT_AND_BACK, Vec4f(0.1, 0.1, 0.1, 1.0));
	material->setDiffuse(Material::FRONT_AND_BACK, Vec4f(0.0, 0.5, 1.0, 1.0));
	material->setSpecular(Material::FRONT_AND_BACK, Vec4f(1.0, 1.0, 1.0, 1.0));
	material->setShininess(Material::FRONT_AND_BACK, 64);
	material->setEmission(Material::FRONT_AND_BACK, Vec4f(0.0, 0.0, 0.0, 1.0));

	stateSet->setAttribute(material);

	ref_ptr<Texture2D> normalmap_texture = new Texture2D();
	normalmap_texture->setDataVariance(osg::Object::DYNAMIC);
	normalmap_texture->setWrap(Texture::WRAP_S, Texture::CLAMP_TO_EDGE);
	normalmap_texture->setFilter(Texture::MIN_FILTER, Texture::LINEAR);
	normalmap_texture->setFilter(Texture::MAG_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
	normalmap_texture->setMaxAnisotropy(8);

	ref_ptr<osg::Image> normalmap_image = osgDB::readImageFile("./data/earth/normal/without_ocean.jpg");
	if (!normalmap_image)
	{
		return stateSet;
	}

	normalmap_texture->setImage(normalmap_image);

	stateSet->setTextureAttributeAndModes(0, normalmap_texture, StateAttribute::ON);

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
		// north
		vertices->push_back(Vec3(0.0, 0.0, radius));
		normals->push_back(Vec3(0.0, 0.0, 1.0));
		texcoords->push_back(Vec2((double)slice / (double)slices, 1.0));

		for (int stack = 1; stack < stacks; stack++)
		{
			Vec3 point(0.0, 0.0, 1.0);

			double pitch = (double)stack * (C_PI / (double)stacks);
			double yaw = (double)slice * (2.0 * C_PI / (double)slices);

			rotateVector(&point, onep::getQuatFromEuler(pitch, 0.0, yaw));

			vertices->push_back(point * radius);
			normals->push_back(point);
			texcoords->push_back(Vec2((double)slice / (double)slices, 1.0 - (double)stack / (double)stacks));
		}

		// south
		vertices->push_back(Vec3(0.0, 0.0, -radius));
		normals->push_back(Vec3(0.0, 0.0, -1.0));
		texcoords->push_back(Vec2((double)slice / (double)slices, 0.0));
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