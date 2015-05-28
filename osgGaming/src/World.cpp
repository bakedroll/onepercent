#include <osgGaming/World.h>
#include <osgGaming/Helper.h>

using namespace std;
using namespace osg;
using namespace osgGaming;

World::World()
{
	_rootNode = new Group();
	_hud = new Hud();
	_cameraManipulator = new TransformableCameraManipulator();

	//_rootNode->addChild(_hud->getProjection());

	initializeStateSet();
}

ref_ptr<Group> World::getRootNode()
{
	return _rootNode;
}

ref_ptr<StateSet> World::getGlobalStateSet()
{
	return _globalStateSet;
}

ref_ptr<LightModel> World::getGlobalLightModel()
{
	return _globalLightModel;
}

osg::ref_ptr<Hud> World::getHud()
{
	return _hud;
}

ref_ptr<TransformableCameraManipulator> World::getCameraManipulator()
{
	return _cameraManipulator;
}

void World::setLightEnabled(int lightNum, bool enabled)
{
	_globalStateSet->setMode(GL_LIGHT0 + lightNum, glModeValueFromBool(enabled));

	getLight(lightNum);
}

ref_ptr<Light> World::getLight(int lightNum)
{
	LightSourceDictionary::iterator it = _lightSources.find(lightNum);

	if (it == _lightSources.end())
	{
		ref_ptr<Light> light = new Light();
		ref_ptr<LightSource> ls = new LightSource();

		light->setLightNum(lightNum);

		ls->setLight(light);

		_rootNode->addChild(ls);

		_lightSources.insert(LightSourceDictPair(lightNum, ls));

		return light;
	}

	return it->second->getLight();
}

void World::initializeStateSet()
{
	_globalStateSet = new StateSet();

	_globalLightModel = new LightModel();

	_globalStateSet->setMode(GL_CULL_FACE, StateAttribute::ON);
	_globalStateSet->setMode(GL_NORMALIZE, StateAttribute::ON);
	_globalStateSet->setMode(GL_DEPTH_TEST, StateAttribute::ON);
	_globalStateSet->setMode(GL_LIGHTING, StateAttribute::ON);

	for (int i = 0; i < OSGGAMING_MAX_LIGHTS; i++)
	{
		_globalStateSet->setMode(GL_LIGHT0 + i, StateAttribute::OFF);
	}

	_globalStateSet->setAttributeAndModes(_globalLightModel, StateAttribute::ON);

	_rootNode->setStateSet(_globalStateSet);
}