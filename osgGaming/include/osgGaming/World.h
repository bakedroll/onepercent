#pragma once

#include <osgGaming/Hud.h>

#include <map>

#include <osg/Referenced>
#include <osg/Group>
#include <osg/Light>
#include <osg/LightModel>
#include <osg/LightSource>

#define OSGGAMING_MAX_LIGHTS 8

namespace osgGaming
{
	typedef std::map<int, osg::ref_ptr<osg::LightSource>> LightSourceDictionary;
	typedef std::pair<int, osg::ref_ptr<osg::LightSource>> LightSourceDictPair;

	class World : public osg::Referenced
	{
	public:
		World();

		osg::ref_ptr<osg::Group> getRootNode();
		osg::ref_ptr<osg::StateSet> getGlobalStateSet();
		osg::ref_ptr<osg::LightModel> getGlobalLightModel();
		osg::ref_ptr<Hud> getHud();

		void setLightEnabled(int lightNum, bool enabled);
		osg::ref_ptr<osg::Light> getLight(int lightNum);

	private:
		void initializeStateSet();

		osg::ref_ptr<osg::Group> _rootNode;
		osg::ref_ptr<osg::StateSet> _globalStateSet;
		osg::ref_ptr<osg::LightModel> _globalLightModel;

		LightSourceDictionary _lightSources;

		osg::ref_ptr<Hud> _hud;
	};
}