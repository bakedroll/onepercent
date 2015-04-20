#include <osgGaming/InputManager.h>

using namespace osgGaming;
using namespace osgGA;
using namespace osg;

InputManager::InputManager(ref_ptr<World> world, ref_ptr<World> worldLoading)
	: osgGA::GUIEventHandler(),
	  _world(world),
	  _worldLoading(worldLoading),
	  _resolutionWidth(0),
	  _resolutionHeight(0)
{

}

bool InputManager::handle(const GUIEventAdapter& ea, GUIActionAdapter&)
{
	switch (ea.getEventType())
	{
	case GUIEventAdapter::KEYDOWN:
	
		if (_currentState.valid())
		{
			_currentState->onKeyHitEvent(ea.getKey());
		}

		return true;

	case GUIEventAdapter::PUSH:

		if (_currentState.valid())
		{
			_currentState->onMouseHitEvent(ea.getButton(), ea.getX(), ea.getY());
		}

		return true;
	
	case GUIEventAdapter::RESIZE:

		if (_graphicsWindow.valid())
		{
			int newWidth = _graphicsWindow->getTraits()->width;
			int newHeight= _graphicsWindow->getTraits()->height;

			if (newWidth != _resolutionWidth || newHeight != _resolutionHeight)
			{
				updateResolution(newWidth, newHeight);
			}
		}

		return true;
	}

	return false;
}

void InputManager::setGraphicsWindow(osg::ref_ptr<osgViewer::GraphicsWindow> graphicsWindow)
{
	_graphicsWindow = graphicsWindow;
}

void InputManager::setCurrentState(osg::ref_ptr<GameState> state)
{
	_currentState = state;
}

void InputManager::updateResolution(unsigned int width, unsigned int height)
{
	_world->getCameraManipulator()->updateResolution(Vec2f(width, height));
	_worldLoading->getCameraManipulator()->updateResolution(Vec2f(width, height));

	_world->getHud()->updateResolution(width, height);
	_worldLoading->getHud()->updateResolution(width, height);

	_resolutionWidth = width;
	_resolutionHeight = height;
}
