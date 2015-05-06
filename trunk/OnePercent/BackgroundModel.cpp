#include "BackgroundModel.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/BlendFunc>
#include <osg/BlendEquation>

#include <osgGaming/ResourceManager.h>

#include <fstream>

using namespace onep;
using namespace osgGaming;
using namespace osg;
using namespace std;
using namespace osgGA;

BackgroundModel::BackgroundModel(string starsFilename)
	: osgGA::GUIEventHandler()
{
	makeStars(starsFilename);

	_transform->addEventCallback(this);
}

ref_ptr<PositionAttitudeTransform> BackgroundModel::getTransform()
{
	return _transform;
}

void BackgroundModel::updateResolutionHeight(float height)
{
	_point->setSize(height / 100.0f);
}

bool BackgroundModel::handle(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case GUIEventAdapter::RESIZE:

		updateResolutionHeight(ea.getWindowHeight());

		return true;
	}

	return false;
}

void BackgroundModel::makeStars(string starsFilename)
{
	char* bytes = ResourceManager::getInstance()->loadBinary(starsFilename);

	_transform = new PositionAttitudeTransform();

	ref_ptr<Geode> geode = new Geode();
	ref_ptr<Geometry> geo = new Geometry();
	ref_ptr<StateSet> stateSet = new StateSet();

	ref_ptr<Vec3Array> verts = new Vec3Array();
	ref_ptr<Vec4Array> colors = new Vec4Array();

	int nstars;
	memcpy(&nstars, bytes, sizeof(int));

	int position = sizeof(int);

	for (int i = 0; i < nstars; i++)
	{
		float size, x, y, z;

		memcpy(&x, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&y, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&z, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&size, &bytes[position], sizeof(float));
		position += sizeof(float);

		verts->push_back(Vec3f(z, x, y) * 10.0f);
		colors->push_back(Vec4f(size / 8.0f, 0.0f, 0.0f, 1.0f));
	}

	_point = new Point();
	ref_ptr<PointSprite> pointSprite = new PointSprite();
	ref_ptr<BlendEquation> blendEquation = new BlendEquation(osg::BlendEquation::FUNC_ADD);

	stateSet->setAttribute(_point);
	stateSet->setTextureAttributeAndModes(0, pointSprite, StateAttribute::ON);
	stateSet->setAttributeAndModes(blendEquation, StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	stateSet->setMode(GL_BLEND, StateAttribute::ON);
	stateSet->setAttributeAndModes(new BlendFunc(GL_ONE, GL_ONE), StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(-10, "RenderBin");

	// shader
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/star.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/star.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	stateSet->setAttribute(pgm, StateAttribute::ON);
	// ###

	geode->setStateSet(stateSet);

	geo->setVertexArray(verts);
	geo->setColorArray(colors);
	geo->setColorBinding(Geometry::BIND_PER_VERTEX);
	geo->addPrimitiveSet(new DrawArrays(PrimitiveSet::POINTS, 0, verts->size()));

	geode->addDrawable(geo);
	_transform->addChild(geode);

	ResourceManager::getInstance()->clearCacheResource(starsFilename);
}