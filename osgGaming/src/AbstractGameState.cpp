#include <osgGaming/AbstractGameState.h>

#include <osgGaming/World.h>
#include <osgGaming/Hud.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/NativeView.h>
#include <osgGaming/UIElement.h>

namespace osgGaming
{
  struct AbstractGameState::Impl
  {
    Impl()
      : initialized(false)
      , firstUpdate(true)
      , worldHudPrepared(false)
      , stateEvent(nullptr)
    {
      for (int i = 0; i < stateBehaviorCount; i++)
        dirty[i] = true;
    }

    static const int stateBehaviorCount = 3;

    bool initialized;
    bool firstUpdate;
    bool worldHudPrepared;
    bool dirty[stateBehaviorCount];

    StateEvent* stateEvent;

    double simulationTime;
    double frameTime;
    osg::ref_ptr<osgGaming::Viewer> viewer;
    osg::ref_ptr<GameSettings> gameSettings;

    // TODO: delete dead views
    typedef std::map<osgGaming::View::WeakPtr, osgGaming::World::Ptr> ViewWorldMap;
    ViewWorldMap overriddenWorlds;

    typedef std::map<osgGaming::View::WeakPtr, osgGaming::Hud::Ptr> ViewHudMap;
    ViewHudMap overriddenHuds;
  };

  AbstractGameState::AbstractGameState()
    : Referenced()
    , m(new Impl())
  {
  }

  AbstractGameState::~AbstractGameState()
  {
  }

  bool AbstractGameState::isInitialized()
  {
    return m->initialized;
  }

  bool AbstractGameState::isWorldAndHudPrepared()
  {
    return m->worldHudPrepared;
  }

  void AbstractGameState::setWorldAndHudPrepared()
  {
    m->worldHudPrepared = true;
  }

  bool AbstractGameState::isFirstUpdate()
  {
    bool first = m->firstUpdate;

    if (first)
    {
      m->firstUpdate = false;
    }

    return first;
  }

  void AbstractGameState::setInitialized()
  {
    m->initialized = true;
  }

  void AbstractGameState::setDirty(StateBehavior behavior)
  {
    m->dirty[int(behavior)] = true;
  }

  bool AbstractGameState::isDirty(StateBehavior behavior)
  {
    bool dirty = m->dirty[int(behavior)];

    m->dirty[int(behavior)] = false;

    return dirty;
  }

  void AbstractGameState::initialize()
  {

  }

  AbstractGameState::StateEvent* AbstractGameState::update()
  {
    return stateEvent_default();
  }

  unsigned char AbstractGameState::getProperties()
  {
    return AbstractGameState::PROP_GUIEVENTS_TOP | AbstractGameState::PROP_UIMEVENTS_TOP | AbstractGameState::PROP_UPDATE_TOP;
  }

  void AbstractGameState::onKeyPressedEvent(int key)
  {

  }

  void AbstractGameState::onKeyReleasedEvent(int key)
  {

  }

  void AbstractGameState::onMousePressedEvent(int button, float x, float y)
  {

  }

  void AbstractGameState::onMouseReleasedEvent(int button, float x, float y)
  {

  }

  void AbstractGameState::onMouseMoveEvent(float x, float y)
  {

  }

  void AbstractGameState::onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion)
  {

  }

  void AbstractGameState::onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change)
  {

  }

  void AbstractGameState::onDragBeginEvent(int button, osg::Vec2f origin)
  {

  }

  void AbstractGameState::onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position)
  {

  }

  void AbstractGameState::onUIClickedEvent(osg::ref_ptr<UIElement> uiElement)
  {

  }

  void AbstractGameState::onResizeEvent(float width, float height)
  {

  }

  void AbstractGameState::prepareWorldAndHud(osg::ref_ptr<View> view)
  {
    Impl::ViewWorldMap::iterator wit = m->overriddenWorlds.find(view);
    World::Ptr world;
    if (wit != m->overriddenWorlds.end())
    {
      world = wit->second;
    }
    else
    {
      world = overrideWorld(view);
      m->overriddenWorlds[view] = world;
    }

    setWorld(view, world);

    Impl::ViewHudMap::iterator hit = m->overriddenHuds.find(view);
    Hud::Ptr hud;
    if (hit != m->overriddenHuds.end())
    {
      hud = hit->second;
    }
    else
    {
      hud = overrideHud(view);
      m->overriddenHuds[view] = hud;
    }

    setHud(view, hud);
  }

  double AbstractGameState::getSimulationTime()
  {
    return m->simulationTime;
  }

  double AbstractGameState::getFrameTime()
  {
    return m->frameTime;
  }

  osg::ref_ptr<World> AbstractGameState::getWorld(osg::ref_ptr<View> view)
  {
    Impl::ViewWorldMap::iterator it = m->overriddenWorlds.find(view);
    if (it != m->overriddenWorlds.end())
      return it->second;

    return nullptr;
  }

  osg::ref_ptr<Hud> AbstractGameState::getHud(osg::ref_ptr<View> view)
  {
    Impl::ViewHudMap::iterator it = m->overriddenHuds.find(view);
    if (it != m->overriddenHuds.end())
      return it->second;

    return nullptr;
  }

  osg::ref_ptr<osgGaming::Viewer> AbstractGameState::getViewer()
  {
    return m->viewer;
  }

  osg::ref_ptr<osgGaming::View> AbstractGameState::getView(int i)
  {
    return m->viewer->getView(i);
  }

  osg::ref_ptr<GameSettings> AbstractGameState::getGameSettings()
  {
    return m->gameSettings;
  }

  void AbstractGameState::setSimulationTime(double simulationTime)
  {
    m->simulationTime = simulationTime;
  }

  void AbstractGameState::setFrameTime(double frameTime)
  {
    m->frameTime = frameTime;
  }

  void AbstractGameState::setWorld(osg::ref_ptr<View> view, osg::ref_ptr<World> world)
  {
    m->overriddenWorlds[view] = world;
  }

  void AbstractGameState::setHud(osg::ref_ptr<View> view, osg::ref_ptr<Hud> hud)
  {
    m->overriddenHuds[view] = hud;
  }

  void AbstractGameState::setViewer(osg::ref_ptr<osgGaming::Viewer> viewer)
  {
    m->viewer = viewer;
  }

  void AbstractGameState::setGameSettings(osg::ref_ptr<GameSettings> settings)
  {
    m->gameSettings = settings;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_default()
  {
    return m->stateEvent;
  }

  osg::ref_ptr<World> AbstractGameState::overrideWorld(osg::ref_ptr<View> view)
  {
    return nullptr;
  }

  osg::ref_ptr<Hud> AbstractGameState::overrideHud(osg::ref_ptr<View> view)
  {
    return nullptr;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_push(osg::ref_ptr<AbstractGameState> state)
  {
    if (m->stateEvent != NULL)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = PUSH;
    m->stateEvent->referencedStates.push_back(state);

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_push(AbstractGameStateList states)
  {
    if (m->stateEvent != NULL)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = PUSH;
    m->stateEvent->referencedStates = states;

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_pop()
  {
    if (m->stateEvent != NULL)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = POP;

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_replace(osg::ref_ptr<AbstractGameState> state)
  {
    if (m->stateEvent != NULL)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = REPLACE;
    m->stateEvent->referencedStates.push_back(state);

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_replace(AbstractGameStateList states)
  {
    if (m->stateEvent != NULL)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = REPLACE;
    m->stateEvent->referencedStates = states;

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_endGame()
  {
    if (m->stateEvent != NULL)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = END_GAME;

    return m->stateEvent;
  }

}
