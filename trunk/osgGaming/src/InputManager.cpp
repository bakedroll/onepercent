#include <osgGaming/InputManager.h>

#include <osgGaming/GameStateStack.h>
#include <osgGaming/Hud.h>
#include <osgGaming/World.h>

#include <unordered_set>

namespace osgGaming
{

struct InputManager::Impl
{
  Impl()
    : mouseDragging(0)
    , gameStateStack(nullptr)
    , isInitialized(false)
  {
    for (int i = 0; i < NUM_MOUSE_BUTTONS; i++)
      mousePressed[i] = false;
  }

  void updateResolution(osg::Vec2f resolution)
  {
    view->updateResolution(resolution);
    updateStates(false);
  }

  void updateStates(bool onlyDirty)
  {
    osg::Vec2f resolution = view->getResolution();

    typedef std::unordered_set<Hud*> HudSet;
    HudSet hudSet;

    gameStateStack->begin(AbstractGameState::ALL);
    while (gameStateStack->next())
    {
      osg::ref_ptr<AbstractGameState> state = gameStateStack->get();

      if (!onlyDirty || state->isDirty(AbstractGameState::ALL))
      {
        osg::ref_ptr<Hud> hud = state->getHud(view);

        HudSet::iterator it = hudSet.find(hud.get());
        if (it == hudSet.end())
        {
          hudSet.insert(hud.get());
          hud->updateResolution(resolution);
        }

        if (gameStateStack->isTop())
          state->getWorld(view)->getCameraManipulator()->updateResolution(resolution);
      }
    }
  }

  int isMousePressed()
  {
    for (int i = 0; i < NUM_MOUSE_BUTTONS; i++)
    {
      if (mousePressed[i] == true)
        return i;
    }

    return -1;
  }

  unsigned int log_x_2(int x)
  {
    return int(log(double(x)) / log(2.0));
  }

  static const unsigned int NUM_MOUSE_BUTTONS = 3;

  osg::ref_ptr<osgGaming::View> view;

  bool mousePressed[NUM_MOUSE_BUTTONS];
  int mouseDragging;
  osg::Vec2f dragOrigin;
  osg::Vec2f lastDragPosition;

  osg::Vec2f mousePosition;

  GameStateStack* gameStateStack;

  bool isInitialized;
};

InputManager::InputManager()
  : osg::Referenced()
  , m(new Impl())
{

}

InputManager::~InputManager()
{
}

void InputManager::setView(osg::ref_ptr<osgGaming::View> viewer)
{
  m->view = viewer;
}

void InputManager::setGameStateStack(GameStateStack* stack)
{
  m->gameStateStack = stack;
}

void InputManager::updateNewRunningStates()
{
  m->updateStates(true);
}

void InputManager::setIsInizialized(bool initialized)
{
  m->isInitialized = initialized;
}

bool InputManager::isInitialized()
{
  return m->isInitialized;
}

bool InputManager::onKeyPressEvent(const osgGA::GUIEventAdapter& ea)
{
  m->gameStateStack->begin(AbstractGameState::GUIEVENT);
  while (m->gameStateStack->next())
  {
    m->gameStateStack->get()->onKeyPressedEvent(ea.getKey());
  }

  return true;
}

bool InputManager::onKeyReleaseEvent(const osgGA::GUIEventAdapter& ea)
{
  m->gameStateStack->begin(AbstractGameState::GUIEVENT);
  while (m->gameStateStack->next())
  {
    m->gameStateStack->get()->onKeyReleasedEvent(ea.getKey());
  }

  return true;
}

bool InputManager::onMouseClickEvent(const osgGA::GUIEventAdapter& ea)
{
  int button = ea.getButton();
  int mb = m->log_x_2(button);

  if (mb >= 0 && mb < Impl::NUM_MOUSE_BUTTONS)
	{
		m->mousePressed[mb] = true;

		m->gameStateStack->begin(AbstractGameState::GUIEVENT);
		while (m->gameStateStack->next())
		{
			m->gameStateStack->get()->onMousePressedEvent(ea.getButton(), ea.getX(), ea.getY());
		}
	}

  return true;
}

bool InputManager::onMouseReleaseEvent(const osgGA::GUIEventAdapter& ea)
{
  int mb = m->log_x_2(ea.getButton());
  if (mb < 0 || mb >= Impl::NUM_MOUSE_BUTTONS)
    return true;

  m->mousePressed[mb] = false;

  if (m->mouseDragging == ea.getButton())
  {
    m->gameStateStack->begin(AbstractGameState::GUIEVENT);
    while (m->gameStateStack->next())
    {
      m->gameStateStack->get()->onDragEndEvent(m->mouseDragging, m->dragOrigin, osg::Vec2f(ea.getX(), ea.getY()));
    }

    m->mouseDragging = 0;
  }

  m->gameStateStack->begin(AbstractGameState::GUIEVENT);
  while (m->gameStateStack->next())
  {
    m->gameStateStack->get()->onMouseReleasedEvent(ea.getButton(), ea.getX(), ea.getY());
  }

  return true;
}

bool InputManager::onMouseMoveEvent(const osgGA::GUIEventAdapter& ea)
{
  m->mousePosition = osg::Vec2f(ea.getX(), ea.getY());

  m->gameStateStack->begin(AbstractGameState::GUIEVENT);
  while (m->gameStateStack->next())
  {
    m->gameStateStack->get()->onMouseMoveEvent(m->mousePosition.x(), m->mousePosition.y());
  }

  if (m->mouseDragging == 0)
  {
    int pressed = m->isMousePressed();

    if (pressed != -1)
    {
      m->mouseDragging = 1 << pressed;
      m->dragOrigin = m->mousePosition;
      m->lastDragPosition = m->dragOrigin;

      m->gameStateStack->begin(AbstractGameState::GUIEVENT);
      while (m->gameStateStack->next())
      {
        m->gameStateStack->get()->onDragBeginEvent(m->mouseDragging, m->dragOrigin);
      }
    }
  }

  if (m->mouseDragging != 0)
  {
    m->gameStateStack->begin(AbstractGameState::GUIEVENT);
    while (m->gameStateStack->next())
    {
      m->gameStateStack->get()->onDragEvent(m->mouseDragging, m->dragOrigin, m->mousePosition, m->mousePosition - m->lastDragPosition);
    }

    m->lastDragPosition = m->mousePosition;
  }

  return true;
}

bool InputManager::onMouseScrollEvent(const osgGA::GUIEventAdapter& ea)
{
  m->gameStateStack->begin(AbstractGameState::GUIEVENT);
  while (m->gameStateStack->next())
  {
    m->gameStateStack->get()->onScrollEvent(ea.getScrollingMotion());
  }

  return true;
}

bool InputManager::onResizeEvent(const osgGA::GUIEventAdapter& ea)
{
  int newWidth = ea.getWindowWidth();
  int newHeight = ea.getWindowHeight();

  int newX = ea.getWindowX();
  int newY = ea.getWindowY();

  osg::Vec2f resolution = m->view->getResolution();

  m->view->updateWindowPosition(osg::Vec2f(float(newX), float(newY)));

  if (newWidth != int(resolution.x()) || newHeight != int(resolution.y()))
  {
    m->updateResolution(osg::Vec2f(float(newWidth), float(newHeight)));

    m->gameStateStack->begin(AbstractGameState::GUIEVENT);
    while (m->gameStateStack->next())
    {
      m->gameStateStack->get()->onResizeEvent(newWidth, newHeight);
    }
  }

  return true;
}

}
