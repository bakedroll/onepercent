#include <osgGaming/InputManager.h>

#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osgViewer;
using namespace osgGA;
using namespace osg;

InputManager::InputManager()
	: osgGA::GUIEventHandler(),
	  _resolutionWidth(0),
	  _resolutionHeight(0),
	  _mouseDragging(0)
{
	for (int i = 0; i < _NUM_MOUSE_BUTTONS; i++)
	{
		_mousePressed[i] = false;
	}
}

bool InputManager::handle(const GUIEventAdapter& ea, GUIActionAdapter& aa)
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

		if (_currentWorld->getHud()->anyUserInteractionModelHovered())
		{
			if (ea.getButton() == GUIEventAdapter::LEFT_MOUSE_BUTTON)
			{
				Hud::UIMList uimList = _currentWorld->getHud()->getUserInteractionModels();
				for (Hud::UIMList::iterator it = uimList.begin(); it != uimList.end(); ++it)
				{
					if ((*it)->getHovered())
					{
						_currentState->onUIMClickedEvent(*it);
					}
				}
			}
		}
		else
		{
			_mousePressed[log_x_2(ea.getButton())] = true;
			_currentState->onMousePressedEvent(ea.getButton(), ea.getX(), ea.getY());
		}

		return true;

	case GUIEventAdapter::RELEASE:

		_mousePressed[log_x_2(ea.getButton())] = false;

		if (_mouseDragging == ea.getButton())
		{
			_currentState->onDragEndEvent(_mouseDragging, _dragOrigin, Vec2f(ea.getX(), ea.getY()));
			handleUserInteractionMove(ea.getX(), ea.getY());

			_mouseDragging = 0;
		}

		if (!_currentWorld->getHud()->anyUserInteractionModelHovered())
		{
			_currentState->onMouseReleasedEvent(ea.getButton(), ea.getX(), ea.getY());
		}

		return true;
	
	case GUIEventAdapter::RESIZE:

		if (_graphicsWindow.valid())
		{
			int newWidth = _graphicsWindow->getTraits()->width;
			int newHeight = _graphicsWindow->getTraits()->height;

			float w = ea.getWindowWidth();

			if (newWidth != _resolutionWidth || newHeight != _resolutionHeight)
			{
				updateResolution(newWidth, newHeight);
				_currentState->onResizeEvent(newWidth, newHeight);
			}
		}

		return true;

	case GUIEventAdapter::DRAG:
	case GUIEventAdapter::MOVE:

		_mousePosition = Vec2f(ea.getX(), ea.getY());

		_currentState->onMouseMoveEvent(_mousePosition.x(), _mousePosition.y());

		if (_mouseDragging == 0)
		{
			handleUserInteractionMove(_mousePosition.x(), _mousePosition.y());
		}

		if (_mouseDragging == 0 && !_currentWorld->getHud()->anyUserInteractionModelHovered())
		{
			int pressed = mousePressed();

			if (pressed != -1)
			{
				_mouseDragging = 1 << pressed;
				_dragOrigin = _mousePosition;
				_lastDragPosition = _dragOrigin;

				_currentState->onDragBeginEvent(_mouseDragging, _dragOrigin);
			}
		}

		if (_mouseDragging != 0)
		{
			_currentState->onDragEvent(_mouseDragging, _dragOrigin, _mousePosition, _mousePosition - _lastDragPosition);

			_lastDragPosition = _mousePosition;
		}

		return true;

	case GUIEventAdapter::SCROLL:

		_currentState->onScrollEvent(ea.getScrollingMotion());
		return true;

	}

	return false;
}

void InputManager::setGraphicsWindow(ref_ptr<GraphicsWindow> graphicsWindow)
{
	_graphicsWindow = graphicsWindow;
}

void InputManager::setCurrentState(osg::ref_ptr<GameState> state)
{
	_currentState = state;
}

void InputManager::setCurrentWorld(ref_ptr<World> world)
{
	_currentWorld = world;

	_currentWorld->getHud()->resetUserInteractionModel();
	handleUserInteractionMove(_mousePosition.x(), _mousePosition.y());
}

void InputManager::updateResolution()
{
	_currentWorld->getHud()->updateResolution(_resolutionWidth, _resolutionHeight);
	_currentWorld->getCameraManipulator()->updateResolution(Vec2f(_resolutionWidth, _resolutionHeight));
}

void InputManager::updateResolution(unsigned int width, unsigned int height)
{
	_resolutionWidth = width;
	_resolutionHeight = height;

	updateResolution();
}

float InputManager::getResolutionWidth()
{
	return _resolutionWidth;
}

float InputManager::getResolutionHeight()
{
	return _resolutionHeight;
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

void InputManager::handleUserInteractionMove(float x, float y)
{
	Hud::UIMList uimList = _currentWorld->getHud()->getUserInteractionModels();

	for (Hud::UIMList::iterator it = uimList.begin(); it != uimList.end(); ++it)
	{
		UserInteractionModel* model = *it;

		Vec2f origin, size;

		model->getAbsoluteOriginSize(origin, size);

		if (pointInRect(Vec2f(x, y), origin, origin + size))
		{
			if (!model->getHovered())
			{
				model->setHovered(true);
				model->onMouseEnter();
			}
		}
		else
		{
			if (model->getHovered())
			{
				model->setHovered(false);
				model->onMouseLeave();
			}
		}
	}
}

unsigned int InputManager::log_x_2(int x)
{
	return int(log((double)x) / log(2.0));
}