#include <osgGaming/GameSettings.h>

using namespace osgGaming;
using namespace osg;

GameSettings::GameSettings()
	: Referenced(),
	_fullscreenEnabled(false),
	_windowResolution(Vec2i(1280, 768)),
	_screenNum(0)
{

}

void GameSettings::setFullscreenEnabled(bool enabled)
{
	_fullscreenEnabled = enabled;
}

void GameSettings::setWindowResolution(Vec2i resolution)
{
	_windowResolution = resolution;
}

void GameSettings::setScreenNum(unsigned int screenNum)
{
	_screenNum = screenNum;
}

bool GameSettings::getFullscreenEnabled()
{
	return _fullscreenEnabled;
}

Vec2i GameSettings::getWindowResolution()
{
	return _windowResolution;
}

unsigned int GameSettings::getScreenNum()
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