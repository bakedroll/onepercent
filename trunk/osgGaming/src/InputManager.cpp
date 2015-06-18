#include <osgGaming/InputManager.h>

#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osgViewer;
using namespace osgGA;
using namespace osg;

InputManager::InputManager()
	: osgGA::GUIEventHandler(),
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
	{
		GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		{
			(*it)->onKeyPressedEvent(ea.getKey());
		}

		return true;
	}
	case GUIEventAdapter::KEYUP:
	{
		GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		{
			(*it)->onKeyReleasedEvent(ea.getKey());
		}

		return true;
	}
	case GUIEventAdapter::PUSH:
	case GUIEventAdapter::DOUBLECLICK:
	{
		bool anyUIMHovered = false;

		GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		{
			if ((*it)->getHud()->anyUserInteractionModelHovered())
			{
				anyUIMHovered = true;

				if (ea.getButton() == GUIEventAdapter::LEFT_MOUSE_BUTTON)
				{
					Hud::UIMList uimList = (*it)->getHud()->getUserInteractionModels();
					for (Hud::UIMList::iterator uit = uimList.begin(); uit != uimList.end(); ++uit)
					{
						if ((*uit)->getHovered())
						{
							(*it)->onUIMClickedEvent(*uit);
						}
					}
				}
			}
			else
			{
				(*it)->onMousePressedEvent(ea.getButton(), ea.getX(), ea.getY());
			}
		}

		if (!anyUIMHovered)
		{
			_mousePressed[log_x_2(ea.getButton())] = true;
		}

		return true;
	}
	case GUIEventAdapter::RELEASE:
	{
		_mousePressed[log_x_2(ea.getButton())] = false;

		GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		if (_mouseDragging == ea.getButton())
		{
			for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
			{
				(*it)->onDragEndEvent(_mouseDragging, _dragOrigin, Vec2f(ea.getX(), ea.getY()));
				handleUserInteractionMove(*it, ea.getX(), ea.getY());
			}

			_mouseDragging = 0;
		}

		for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		{
			if (!(*it)->getHud()->anyUserInteractionModelHovered())
			{
				(*it)->onMouseReleasedEvent(ea.getButton(), ea.getX(), ea.getY());
			}
		}

		return true;
	}
	case GUIEventAdapter::RESIZE:
	{
		int newWidth = ea.getWindowWidth();
		int newHeight = ea.getWindowHeight();

		int newX = ea.getWindowX();
		int newY = ea.getWindowY();

		Vec2f resolution = _viewer->getResolution();

		_viewer->updateWindowPosition(Vec2f((float)newX, (float)newY));

		if (newWidth != (int)resolution.x() || newHeight != (int)resolution.y())
		{
			updateResolution(Vec2f((float)newWidth, (float)newHeight));

			GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

			for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
			{
				(*it)->onResizeEvent(newWidth, newHeight);
			}
		}

		return true;
	}
	case GUIEventAdapter::DRAG:
	case GUIEventAdapter::MOVE:
	{
		bool anyUIMHovered = false;

		_mousePosition = Vec2f(ea.getX(), ea.getY());

		GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		{
			(*it)->onMouseMoveEvent(_mousePosition.x(), _mousePosition.y());

			if (_mouseDragging == 0)
			{
				handleUserInteractionMove(*it, _mousePosition.x(), _mousePosition.y());
			}

			if ((*it)->getHud()->anyUserInteractionModelHovered())
			{
				anyUIMHovered = true;
			}
		}


		if (_mouseDragging == 0 && !anyUIMHovered)
		{
			int pressed = mousePressed();

			if (pressed != -1)
			{
				_mouseDragging = 1 << pressed;
				_dragOrigin = _mousePosition;
				_lastDragPosition = _dragOrigin;

				for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
				{
					(*it)->onDragBeginEvent(_mouseDragging, _dragOrigin);
				}
			}
		}

		if (_mouseDragging != 0)
		{
			for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
			{
				(*it)->onDragEvent(_mouseDragging, _dragOrigin, _mousePosition, _mousePosition - _lastDragPosition);
			}

			_lastDragPosition = _mousePosition;
		}

		return true;
	}
	case GUIEventAdapter::SCROLL:
	{
		GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		{
			(*it)->onScrollEvent(ea.getScrollingMotion());
		}

		return true;
	}
	}

	return false;
}

void InputManager::setViewer(ref_ptr<osgGaming::Viewer> viewer)
{
	_viewer = viewer;
}

void InputManager::setGameStateStack(GameStateStack* stack)
{
	_gameStateStack = stack;
}

void InputManager::updateNewRunningStates()
{
	GameStateStack::AbstractGameStateList* newRunningStates = _gameStateStack->getNewRunningStates();

	if (newRunningStates->empty())
	{
		return;
	}

	Vec2f resolution = _viewer->getResolution();

	ref_ptr<Hud> currentHud;
	ref_ptr<World> currentWorld;

	for (GameStateStack::AbstractGameStateList::iterator it = newRunningStates->begin(); it != newRunningStates->end(); ++it)
	{
		if (currentHud != it->get()->getHud())
		{
			currentHud = it->get()->getHud();

			currentHud->updateResolution(resolution);
			currentHud->resetUserInteractionModel();

			handleUserInteractionMove((*it), _mousePosition.x(), _mousePosition.y());
		}

		if (currentWorld != it->get()->getWorld())
		{
			currentWorld = it->get()->getWorld();

			currentWorld->getCameraManipulator()->updateResolution(resolution);
		}
	}
}

void InputManager::updateResolution(Vec2f resolution)
{
	_viewer->updateResolution(resolution);

	GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

	if (runningStates->empty())
	{
		return;
	}

	ref_ptr<Hud> currentHud;
	ref_ptr<World> currentWorld;

	for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
	{
		if (currentHud != it->get()->getHud())
		{
			currentHud = it->get()->getHud();

			currentHud->updateResolution(resolution);
		}

		if (currentWorld != it->get()->getWorld())
		{
			currentWorld = it->get()->getWorld();

			currentWorld->getCameraManipulator()->updateResolution(resolution);
		}
	}
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

void InputManager::handleUserInteractionMove(osg::ref_ptr<AbstractGameState> state, float x, float y)
{
	Hud::UIMList uimList = state->getHud()->getUserInteractionModels();

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