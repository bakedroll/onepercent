#pragma once

#include <osgGaming/TransformableCameraManipulator.h>

#include <memory>
#include <map>

#include <osg/Referenced>
#include <osg/Group>
#include <osg/Light>
#include <osg/LightModel>
#include <osg/LightSource>

namespace osgGaming
{
  constexpr auto numMaxLights = 8;

  class Injector;

  using LightSourceDictionary = std::map<int, osg::ref_ptr<osg::LightSource>>;
	using LightSourceDictPair = std::pair<int, osg::ref_ptr<osg::LightSource>>;

	class World : public osg::Referenced
	{
	public:
    using Ptr = osg::ref_ptr<World>;

		World(Injector& injector);
    ~World();

    osg::ref_ptr<osg::Group>                     getRootNode() const;
    osg::ref_ptr<osg::StateSet>                  getGlobalStateSet() const;
    osg::ref_ptr<osg::LightModel>                getGlobalLightModel() const;
    osg::ref_ptr<TransformableCameraManipulator> getCameraManipulator() const;

    void setLightEnabled(int lightNum, bool enabled);
		osg::ref_ptr<osg::Light> getLight(int lightNum) const;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}