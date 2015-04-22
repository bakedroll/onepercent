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
		void setScreenNum(unsigned int screenNum);

		bool getFullscreenEnabled();
		osg::Vec2i getWindowResolution();
		unsigned int getScreenNum();

		void load();
		void save();

	private:
		bool _fullscreenEnabled;
		osg::Vec2i _windowResolution;
		unsigned int _screenNum;

		bool _isLoaded;
	};
}