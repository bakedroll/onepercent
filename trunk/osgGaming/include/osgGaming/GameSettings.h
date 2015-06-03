#pragma once

#include <osg/Referenced>
#include <osg/Vec2f>

namespace osgGaming
{
	class GameSettings : public osg::Referenced
	{
	public:
		GameSettings();

		void setFullscreenEnabled(bool enabled);
		void setWindowResolution(osg::Vec2f resolution);
		void setScreenNum(unsigned int screenNum);

		bool getFullscreenEnabled();
		osg::Vec2f getWindowedResolution();
		int getScreenNum();

		void load();
		void save();

	private:
		bool _fullscreenEnabled;
		osg::Vec2f _windowedResolution;
		int _screenNum;

		bool _isLoaded;
	};
}