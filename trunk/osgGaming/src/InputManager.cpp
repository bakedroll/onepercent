#include <osgGaming/InputManager.h>

using namespace osgGaming;
using namespace osgGA;
using namespace osg;

InputManager::InputManager(ref_ptr<World> world, ref_ptr<World> worldLoading)
	: osgGA::GUIEventHandler(),
	  _world(world),
	  _worldLoading(worldLoading),
	  _resolutionWidth(0),
	  _resolutionHeight(0),
	  _mouseDragging(0)
{
	for (int i = 0; i < _NUM_MOUSE_BUTTONS; i++)
	{
		_mousePressed[i] = false;
	}
}

bool InputManager::handle(const GUIEventAdapter& ea, GUIActionAdapter&)
{
	switch (ea.getEventType())
	{
	case GUIEventAdapter::KEYDOWN:
	
		_currentState->onKeyPressedEvent(ea.getKey());

		return true;

	case GUIEventAdapter::KEYUP:

		_currentState->onKeyReleasedEvent(ea.getKey());

		return true;

	case GUIEventAdapter::PUSH:
	case GUIEventAdapter::DOUBLECLICK:

		_mousePressed[log_x_2(ea.getButton())] = true;

		_currentState->onMousePressedEvent(ea.getButton(), ea.getX(), ea.getY());

		return true;

	case GUIEventAdapter::RELEASE:

		_mousePressed[log_x_2(ea.getButton())] = false;

		if (_mouseDragging == ea.getButton())
		{
			_currentState->onDragEndEvent(_mouseDragging, _dragOrigin, Vec2f(ea.getX(), ea.getY()));

			_mouseDragging = 0;
		}

		_currentState->onMouseReleasedEvent(ea.getButton(), ea.getX(), ea.getY());

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

	case GUIEventAdapter::DRAG:
	case GUIEventAdapter::MOVE:

		_currentState->onMouseMoveEvent(ea.getX(), ea.getY());

		if (_mouseDragging == 0)
		{
			int pressed = mousePressed();

			if (pressed != -1)
			{
				_mouseDragging = 1 << pressed;
				_dragOrigin = Vec2f(ea.getX(), ea.getY());

				_currentState->onDragBeginEvent(_mouseDragging, _dragOrigin);
			}
		}

		if (_mouseDragging != 0)
		{
			_currentState->onDragEvent(_mouseDragging, _dragOrigin, Vec2f(ea.getX(), ea.getY()));
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

int InputManager::mousePressed()
{
	for (int i = 0; i < _NUM_MOUSE_BUTTONS; i++)
	{
		if (_mousePressed[i] == true)
			return i;
	}

	return -1;
}

unsigned int InputManager::log_x_2(int x)
{
	return int(log((double)x) / log(2.0));
}