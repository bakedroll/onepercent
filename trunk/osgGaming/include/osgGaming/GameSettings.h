#pragma once

#include <osg/Referenced>
#include <osg/Vec2d>

namespace osgGaming
{
	class GameSettings : public osg::Referenced
	{
	public:
		GameSettings();

		void setFullscreenEnabled(bool enabled);
		void setWindowResolution(osg::Vec2d resolution);

		bool getFullscreenEnabled();
		osg::Vec2d getWindowResolution();


		void load();
		void save();

	private:
		bool _fullscreenEnabled;
		osg::Vec2d _windowResolution;

		bool _isLoaded;
	};
}