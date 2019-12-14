#include <osgGaming/World.h>
#include <osgGaming/Helper.h>

namespace osgGaming
{
  struct World::Impl
  {
    Impl()
    {
      rootNode          = new osg::Group();
      cameraManipulator = new TransformableCameraManipulator();

      initializeStateSet();
    }

    void initializeStateSet()
    {
      globalStateSet   = new osg::StateSet();
      globalLightModel = new osg::LightModel();

      globalStateSet->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
      globalStateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
      globalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      globalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

      for (auto i = 0; i < numMaxLights; i++)
      {
        globalStateSet->setMode(GL_LIGHT0 + i, osg::StateAttribute::OFF);
      }

      globalStateSet->setAttributeAndModes(globalLightModel, osg::StateAttribute::ON);
      rootNode->setStateSet(globalStateSet);
    }

    osg::ref_ptr<osg::Group>      rootNode;
    osg::ref_ptr<osg::StateSet>   globalStateSet;
    osg::ref_ptr<osg::LightModel> globalLightModel;

    LightSourceDictionary                        lightSources;
    osg::ref_ptr<TransformableCameraManipulator> cameraManipulator;
  };

  World::World(Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  World::~World() = default;

  osg::ref_ptr<osg::Group> World::getRootNode() const
  {
    return m->rootNode;
  }

  osg::ref_ptr<osg::StateSet> World::getGlobalStateSet() const
  {
    return m->globalStateSet;
  }

  osg::ref_ptr<osg::LightModel> World::getGlobalLightModel() const
  {
    return m->globalLightModel;
  }

  osg::ref_ptr<TransformableCameraManipulator> World::getCameraManipulator() const
  {
    return m->cameraManipulator;
  }

  void World::setLightEnabled(int lightNum, bool enabled)
  {
    m->globalStateSet->setMode(GL_LIGHT0 + lightNum, glModeValueFromBool(enabled));

    getLight(lightNum);
  }

  osg::ref_ptr<osg::Light> World::getLight(int lightNum) const
  {
    const auto it = m->lightSources.find(lightNum);
    if (it == m->lightSources.end())
    {
      auto light       = new osg::Light();
      auto lightSource = new osg::LightSource();

      light->setLightNum(lightNum);
      lightSource->setLight(light);

      m->rootNode->addChild(lightSource);
      m->lightSources[lightNum] = lightSource;

      return light;
    }

    return it->second->getLight();
  }

}