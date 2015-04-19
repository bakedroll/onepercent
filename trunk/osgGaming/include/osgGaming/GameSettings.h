#pragma once

#include <osg/Referenced>
#include <osg/Vec2i>

namespace osgGaming
{
	class GameSettings : public osg::Referenced
	{
	public:
		GameSettings();

		void setFullscreenEnabled(bool enabled);
		void setWindowResolution(osg::Vec2i resolution);

		bool getFullscreenEnabled();
		osg::Vec2i getWindowResolution();


		void load();
		void save();

	private:
		bool _fullscreenEnabled;
		osg::Vec2i _windowResolution;

		bool _isLoaded;
	};
}