#include "BackgroundModel.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/Billboard>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>
#include <osgGaming/Helper.h>
#include <osgGaming/ByteStream.h>

using namespace onep;
using namespace osgGaming;
using namespace osg;
using namespace std;
using namespace osgGA;

BackgroundModel::BackgroundModel()
	: GUIEventHandler()
{
	makeStars();
	makeSun();

	_transform->addEventCallback(this);
}

ref_ptr<PositionAttitudeTransform> BackgroundModel::getTransform()
{
	return _transform;
}

ref_ptr<PositionAttitudeTransform> BackgroundModel::getSunTransform()
{
	return _sunTransform;
}

ref_ptr<PositionAttitudeTransform> BackgroundModel::getSunGlowTransform()
{
	return _sunGlowTransform;
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

	default:
		break;

	}

	return false;
}

void BackgroundModel::makeStars()
{
	string starsFilename = "./GameData/data/stars.bin";

	char* bytes = ResourceManager::getInstance()->loadBinary(starsFilename);

	_transform = new PositionAttitudeTransform();

	ref_ptr<Geode> geode = new Geode();
	ref_ptr<Geometry> geo = new Geometry();
	ref_ptr<StateSet> globStateSet = new StateSet();
	ref_ptr<StateSet> stateSet = new StateSet();

	ref_ptr<Vec3Array> verts = new Vec3Array();
	ref_ptr<Vec4Array> colors = new Vec4Array();

	ByteStream stream(bytes);

	int nstars = stream.read<int>();

	for (int i = 0; i < nstars; i++)
	{
		float x = stream.read<float>();
		float y = stream.read<float>();
		float z = stream.read<float>();
		float size = stream.read<float>();

		verts->push_back(Vec3f(x, -z, -y) * 10.0f);
		colors->push_back(Vec4f(size / 8.0f, 0.0f, 0.0f, 1.0f));
	}

	_point = new Point();
	ref_ptr<PointSprite> pointSprite = new PointSprite();
	ref_ptr<BlendEquation> blendEquation = new BlendEquation(BlendEquation::FUNC_ADD);

	stateSet->setAttribute(_point);
	stateSet->setTextureAttributeAndModes(0, pointSprite, StateAttribute::ON);

	globStateSet->setAttributeAndModes(blendEquation, StateAttribute::ON);
	globStateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	globStateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	globStateSet->setMode(GL_BLEND, StateAttribute::ON);
	globStateSet->setAttributeAndModes(new BlendFunc(GL_ONE, GL_ONE), StateAttribute::ON);
	stateSet->setRenderBinDetails(-10, "RenderBin");

	// shader
	ref_ptr<Program> pgm = new Program();

	ref_ptr<Shader> vert_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/star.vert", Shader::VERTEX);
	ref_ptr<Shader> frag_shader = ResourceManager::getInstance()->loadShader("./GameData/shaders/star.frag", Shader::FRAGMENT);

	pgm->addShader(vert_shader);
	pgm->addShader(frag_shader);

	stateSet->setAttribute(pgm, StateAttribute::ON);
	// ###

	_transform->setStateSet(globStateSet);
	geode->setStateSet(stateSet);

	geo->setVertexArray(verts);
	geo->setColorArray(colors);
	geo->setColorBinding(Geometry::BIND_PER_VERTEX);
	geo->addPrimitiveSet(new DrawArrays(PrimitiveSet::POINTS, 0, verts->size()));

	geode->addDrawable(geo);
	_transform->addChild(geode);

	ResourceManager::getInstance()->clearCacheResource(starsFilename);
}

void BackgroundModel::makeSun()
{
	ref_ptr<Billboard> sunBillboard = new Billboard();
	_sunTransform = new PositionAttitudeTransform();
	_sunGlowTransform = new PositionAttitudeTransform();
	ref_ptr<PositionAttitudeTransform> sunPosTransform = new PositionAttitudeTransform();
	ref_ptr<StateSet> stateSet = new StateSet();

	sunBillboard->setMode(Billboard::Mode::POINT_ROT_EYE);
	sunBillboard->setNormal(Vec3(0.0f, -1.0f, 0.0f));

	ref_ptr<Geometry> geo = createQuadGeometry(-1.0f, 1.0f, -1.0f, 1.0f);

	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(Vec4f(1.0, 1.0, 1.0, 1.0));
	geo->setColorArray(colors);
	geo->setColorBinding(Geometry::BIND_OVERALL);

	sunPosTransform->setPosition(Vec3f(0.0f, 9.0f, 0.0f));
	sunPosTransform->setScale(Vec3f(0.3f, 0.3f, 0.3f));

	TextureFactory::getInstance()->make()
		->image(ResourceManager::getInstance()->loadImage("./GameData/textures/sun/sprite.png"))
		->assign(stateSet)
		->build();

	TextureFactory::getInstance()->make()
		->image(ResourceManager::getInstance()->loadImage("./GameData/textures/sun/blend.png"))
		->assign(_sunGlowTransform->getOrCreateStateSet())
		->build();

	sunBillboard->getOrCreateStateSet()->setRenderBinDetails(-10, "RenderBin");
	_sunGlowTransform->getOrCreateStateSet()->setRenderBinDetails(10, "RenderBin");

	sunPosTransform->setStateSet(stateSet);

	_transform->addChild(_sunTransform);
	_sunTransform->addChild(sunPosTransform);
	sunPosTransform->addChild(sunBillboard);
	sunPosTransform->addChild(_sunGlowTransform);
	_sunGlowTransform->addChild(sunBillboard);
	sunBillboard->addDrawable(geo);
}