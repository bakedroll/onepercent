#include <osgGaming/GameSettings.h>

using namespace osgGaming;
using namespace osg;

GameSettings::GameSettings()
	: Referenced(),
	_fullscreenEnabled(false),
	_windowedResolution(Vec2f(1280.0f, 768.0f)),
	_screenNum(0)
{

}

void GameSettings::setFullscreenEnabled(bool enabled)
{
	_fullscreenEnabled = enabled;
}

void GameSettings::setWindowResolution(Vec2f resolution)
{
	_windowedResolution = resolution;
}

void GameSettings::setScreenNum(unsigned int screenNum)
{
	_screenNum = screenNum;
}

bool GameSettings::getFullscreenEnabled()
{
	return _fullscreenEnabled;
}

Vec2f GameSettings::getWindowedResolution()
{
	return _windowedResolution;
}

int GameSettings::getScreenNum()
{
	return _screenNum;
}

void GameSettings::load()
{
	if (_isLoaded)
	{
		return;
	}

	_isLoaded = true;
}

void GameSettings::save()
{

}