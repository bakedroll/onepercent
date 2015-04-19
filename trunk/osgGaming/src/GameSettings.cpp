#include "GameSettings.h"

using namespace osgGaming;
using namespace osg;

GameSettings::GameSettings()
	: Referenced(),
	_fullscreenEnabled(false),
	_windowResolution(Vec2d(1280, 768))
{

}

void GameSettings::setFullscreenEnabled(bool enabled)
{
	_fullscreenEnabled = enabled;
}

void GameSettings::setWindowResolution(Vec2d resolution)
{
	_windowResolution = resolution;
}

bool GameSettings::getFullscreenEnabled()
{
	return _fullscreenEnabled;
}

Vec2d GameSettings::getWindowResolution()
{
	return _windowResolution;
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