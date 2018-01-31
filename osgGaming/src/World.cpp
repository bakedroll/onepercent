#include <osgGaming/World.h>
#include <osgGaming/Helper.h>

namespace osgGaming
{
  struct World::Impl
  {
    Impl()
    {
      rootNode = new osg::Group();
      cameraManipulator = new TransformableCameraManipulator();

      initializeStateSet();
    }

    void initializeStateSet()
    {
      globalStateSet = new osg::StateSet();

      globalLightModel = new osg::LightModel();

      globalStateSet->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
      globalStateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
      globalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      globalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

      for (int i = 0; i < OSGGAMING_MAX_LIGHTS; i++)
      {
        globalStateSet->setMode(GL_LIGHT0 + i, osg::StateAttribute::OFF);
      }

      globalStateSet->setAttributeAndModes(globalLightModel, osg::StateAttribute::ON);

      rootNode->setStateSet(globalStateSet);
    }

    osg::ref_ptr<osg::Group> rootNode;
    osg::ref_ptr<osg::StateSet> globalStateSet;
    osg::ref_ptr<osg::LightModel> globalLightModel;

    LightSourceDictionary lightSources;

    osg::ref_ptr<TransformableCameraManipulator> cameraManipulator;
  };

  World::World(Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  World::~World()
  {
  }

  osg::ref_ptr<osg::Group> World::getRootNode()
  {
    return m->rootNode;
  }

  osg::ref_ptr<osg::StateSet> World::getGlobalStateSet()
  {
    return m->globalStateSet;
  }

  osg::ref_ptr<osg::LightModel> World::getGlobalLightModel()
  {
    return m->globalLightModel;
  }

  osg::ref_ptr<TransformableCameraManipulator> World::getCameraManipulator()
  {
    return m->cameraManipulator;
  }

  void World::setLightEnabled(int lightNum, bool enabled)
  {
    m->globalStateSet->setMode(GL_LIGHT0 + lightNum, glModeValueFromBool(enabled));

    getLight(lightNum);
  }

  osg::ref_ptr<osg::Light> World::getLight(int lightNum)
  {
    LightSourceDictionary::iterator it = m->lightSources.find(lightNum);

    if (it == m->lightSources.end())
    {
      osg::ref_ptr<osg::Light> light = new osg::Light();
      osg::ref_ptr<osg::LightSource> ls = new osg::LightSource();

      light->setLightNum(lightNum);

      ls->setLight(light);

      m->rootNode->addChild(ls);

      m->lightSources.insert(LightSourceDictPair(lightNum, ls));

      return light;
    }

    return it->second->getLight();
  }

}