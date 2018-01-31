#pragma once

#include <osgGaming/TransformableCameraManipulator.h>

#include <memory>
#include <map>

#include <osg/Referenced>
#include <osg/Group>
#include <osg/Light>
#include <osg/LightModel>
#include <osg/LightSource>

#define OSGGAMING_MAX_LIGHTS 8

namespace osgGaming
{
  class Injector;

  typedef std::map<int, osg::ref_ptr<osg::LightSource>> LightSourceDictionary;
	typedef std::pair<int, osg::ref_ptr<osg::LightSource>> LightSourceDictPair;

	class World : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<World> Ptr;

		World(Injector& injector);
    ~World();

		osg::ref_ptr<osg::Group> getRootNode();
		osg::ref_ptr<osg::StateSet> getGlobalStateSet();
		osg::ref_ptr<osg::LightModel> getGlobalLightModel();
		osg::ref_ptr<TransformableCameraManipulator> getCameraManipulator();

		void setLightEnabled(int lightNum, bool enabled);
		osg::ref_ptr<osg::Light> getLight(int lightNum);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}