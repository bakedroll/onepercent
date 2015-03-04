#include "GlobeModel.h"

using namespace onep;

GlobeModel::GlobeModel()
{
	ref_ptr<Vec4Array> cAry = new Vec4Array();
	setColorArray(cAry, osg::Array::BIND_OVERALL);
	cAry->push_back(osg::Vec4(1, 1, 1, 1));

	ref_ptr<Vec3Array> vAry = new Vec3Array();
	setVertexArray(vAry);
	vAry->push_back(Vec3(0, 0, 0));

	addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, vAry->size()));

	osg::StateSet* sset = getOrCreateStateSet();

	sset->setAttribute(createShader());

	setInitialBound(BoundingBox(Vec3(-10, -10, -10), Vec3(10, 10, 10)));
}

ref_ptr<Program> GlobeModel::createShader()
{
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = new Shader(Shader::VERTEX);
	ref_ptr<Shader> geom_shader = new Shader(Shader::GEOMETRY);
	ref_ptr<Shader> frag_shader = new Shader(Shader::FRAGMENT);

	vert_shader->loadShaderSourceFromFile("./shader/globe.vert");
	geom_shader->loadShaderSourceFromFile("./shader/globe.geom");
	frag_shader->loadShaderSourceFromFile("./shader/globe.frag");

	pgm->addShader(vert_shader);
	pgm->addShader(geom_shader);
	pgm->addShader(frag_shader);

	//pgm->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 4);
	//pgm->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
	//pgm->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);

	return pgm;
}