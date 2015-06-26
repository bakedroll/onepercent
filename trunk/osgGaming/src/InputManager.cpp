#include <osgGaming/InputManager.h>

#include <unordered_set>
#include <osgGaming/Helper.h>

using namespace osgGaming;
using namespace osgViewer;
using namespace osgGA;
using namespace osg;
using namespace std;

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
		_gameStateStack->begin(AbstractGameState::GUIEVENT);
		while (_gameStateStack->next())
		{
			_gameStateStack->get()->onKeyPressedEvent(ea.getKey());
		}

		return true;
	}
	case GUIEventAdapter::KEYUP:
	{
		_gameStateStack->begin(AbstractGameState::GUIEVENT);
		while (_gameStateStack->next())
		{
			_gameStateStack->get()->onKeyReleasedEvent(ea.getKey());
		}

		return true;
	}
	case GUIEventAdapter::PUSH:
	case GUIEventAdapter::DOUBLECLICK:
	{
		bool anyUIMHovered = false;

		//GameStateStack::AbstractGameStateList* runningStates = _gameStateStack->getRunningStates();

		//for (GameStateStack::AbstractGameStateList::iterator it = runningStates->begin(); it != runningStates->end(); ++it)
		_gameStateStack->begin(AbstractGameState::UIMEVENT);
		while (_gameStateStack->next())
		{
			ref_ptr<AbstractGameState> state = _gameStateStack->get();

			if (state->getHud()->anyUserInteractionModelHovered())
			{
				anyUIMHovered = true;

				if (ea.getButton() == GUIEventAdapter::LEFT_MOUSE_BUTTON)
				{
					Hud::UIMList uimList = state->getHud()->getUserInteractionModels();
					for (Hud::UIMList::iterator uit = uimList.begin(); uit != uimList.end(); ++uit)
					{
						if ((*uit)->getHovered())
						{
							state->onUIMClickedEvent(*uit);
						}
					}
				}
			}
		}

		if (!anyUIMHovered)
		{
			_mousePressed[log_x_2(ea.getButton())] = true;

			_gameStateStack->begin(AbstractGameState::GUIEVENT);
			while (_gameStateStack->next())
			{
				_gameStateStack->get()->onMousePressedEvent(ea.getButton(), ea.getX(), ea.getY());
			}
		}

		return true;
	}
	case GUIEventAdapter::RELEASE:
	{
		_mousePressed[log_x_2(ea.getButton())] = false;

		if (_mouseDragging == ea.getButton())
		{
			_gameStateStack->begin(AbstractGameState::GUIEVENT);
			while (_gameStateStack->next())
			{
				_gameStateStack->get()->onDragEndEvent(_mouseDragging, _dragOrigin, Vec2f(ea.getX(), ea.getY()));
			}

			_gameStateStack->begin(AbstractGameState::UIMEVENT);
			while (_gameStateStack->next())
			{
				handleUserInteractionMove(_gameStateStack->get(), ea.getX(), ea.getY());
			}

			_mouseDragging = 0;
		}

		_gameStateStack->begin(AbstractGameState::GUIEVENT);
		while (_gameStateStack->next())
		{
			if (!_gameStateStack->get()->getHud()->anyUserInteractionModelHovered())
			{
				_gameStateStack->get()->onMouseReleasedEvent(ea.getButton(), ea.getX(), ea.getY());
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

			_gameStateStack->begin(AbstractGameState::GUIEVENT);
			while (_gameStateStack->next())
			{
				_gameStateStack->get()->onResizeEvent(newWidth, newHeight);
			}
		}

		return true;
	}
	case GUIEventAdapter::DRAG:
	case GUIEventAdapter::MOVE:
	{
		bool anyUIMHovered = false;

		_mousePosition = Vec2f(ea.getX(), ea.getY());

		_gameStateStack->begin(AbstractGameState::GUIEVENT);
		while (_gameStateStack->next())
		{
			_gameStateStack->get()->onMouseMoveEvent(_mousePosition.x(), _mousePosition.y());
		}

		_gameStateStack->begin(AbstractGameState::UIMEVENT);
		while (_gameStateStack->next())
		{
			if (_mouseDragging == 0)
			{
				handleUserInteractionMove(_gameStateStack->get(), _mousePosition.x(), _mousePosition.y());
			}

			if (_gameStateStack->get()->getHud()->anyUserInteractionModelHovered())
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

				_gameStateStack->begin(AbstractGameState::GUIEVENT);
				while (_gameStateStack->next())
				{
					_gameStateStack->get()->onDragBeginEvent(_mouseDragging, _dragOrigin);
				}
			}
		}

		if (_mouseDragging != 0)
		{
			_gameStateStack->begin(AbstractGameState::GUIEVENT);
			while (_gameStateStack->next())
			{
				_gameStateStack->get()->onDragEvent(_mouseDragging, _dragOrigin, _mousePosition, _mousePosition - _lastDragPosition);
			}

			_lastDragPosition = _mousePosition;
		}

		return true;
	}
	case GUIEventAdapter::SCROLL:
	{
		_gameStateStack->begin(AbstractGameState::GUIEVENT);
		while (_gameStateStack->next())
		{
			_gameStateStack->get()->onScrollEvent(ea.getScrollingMotion());
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
	updateStates(true, false);
}

void InputManager::updateResolution(Vec2f resolution)
{
	_viewer->updateResolution(resolution);

	updateStates(false, true);
}

void InputManager::updateStates(bool onlyDirty, bool onlyResolution)
{
	Vec2f resolution = _viewer->getResolution();

	typedef unordered_set<Hud*> HudSet;
	HudSet hudSet;

	_gameStateStack->begin(AbstractGameState::UIMEVENT);
	while (_gameStateStack->next())
	{
		ref_ptr<AbstractGameState> state = _gameStateStack->get();
		ref_ptr<Hud> hud = state->getHud();

		HudSet::iterator it = hudSet.find(hud.get());
		if (it == hudSet.end())
		{
			hudSet.insert(hud.get());

			hud->updateResolution(resolution);

			if (!onlyResolution || state->isDirty(AbstractGameState::UIMEVENT))
			{
				hud->resetUserInteractionModel();
				handleUserInteractionMove(state, _mousePosition.x(), _mousePosition.y());
			}
		}
	}

	_gameStateStack->top()->getWorld()->getCameraManipulator()->updateResolution(resolution);
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