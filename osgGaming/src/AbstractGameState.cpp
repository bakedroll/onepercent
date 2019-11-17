#include <osgGaming/AbstractGameState.h>

#include <osgGaming/World.h>
#include <osgGaming/Hud.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/View.h>
#include <osgGaming/Helper.h>

namespace osgGaming
{
  struct AbstractGameState::Impl
  {
    Impl()
      : initialized(false)
      , firstUpdate(true)
      , worldHudPrepared(false)
      , stateEvent(nullptr)
      , simulationTime(0.0)
      , frameTime(0.0)
    {
      for (auto& i : dirty)
      {
        i = true;
	    }
    }

    static const int stateBehaviorCount = 3;

    bool initialized;
    bool firstUpdate;
    bool worldHudPrepared;
    bool dirty[stateBehaviorCount];

    StateEvent* stateEvent;

    double                     simulationTime;
    double                     frameTime;
    osg::ref_ptr<Viewer>       viewer;
    osg::ref_ptr<GameSettings> gameSettings;

    // TODO: delete dead views
    using ViewWorldMap = std::map<View::WeakPtr, World::Ptr>;
    ViewWorldMap overriddenWorlds;

    using ViewHudMap = std::map<View::WeakPtr, Hud::Ptr>;
    ViewHudMap overriddenHuds;
  };

  AbstractGameState::AbstractGameState()
    : Referenced()
    , m(new Impl())
    , m_injector(nullptr)
  {
  }

  AbstractGameState::~AbstractGameState() = default;

  bool AbstractGameState::isInitialized() const
  {
    return m->initialized;
  }

  bool AbstractGameState::isWorldAndHudPrepared() const
  {
    return m->worldHudPrepared;
  }

  void AbstractGameState::setWorldAndHudPrepared()
  {
    m->worldHudPrepared = true;
  }

  bool AbstractGameState::isFirstUpdate() const
  {
    auto first = m->firstUpdate;
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

  bool AbstractGameState::isDirty(StateBehavior behavior) const
  {
    auto dirty              = m->dirty[int(behavior)];
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

  unsigned char AbstractGameState::getProperties() const
  {
    return (underlying(StateProperties::PROP_GUIEVENTS_TOP) | underlying(StateProperties::PROP_UPDATE_TOP));
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

  void AbstractGameState::onDragEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position,
                                      const osg::Vec2f& change)
  {
  }

  void AbstractGameState::onDragBeginEvent(int button, const osg::Vec2f& origin)
  {
  }

  void AbstractGameState::onDragEndEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position)
  {
  }

  void AbstractGameState::onResizeEvent(float width, float height)
  {

  }

  void AbstractGameState::prepareWorldAndHud(Injector& injector, const osg::ref_ptr<View>& view)
  {
    auto wit = m->overriddenWorlds.find(view);
    World::Ptr world;
    if (wit != m->overriddenWorlds.end())
    {
      world = wit->second;
    }
    else
    {
      world = injectWorld(injector, view);
      m->overriddenWorlds[view] = world;
    }

    setWorld(view, world);

    auto hit = m->overriddenHuds.find(view);
    Hud::Ptr hud;
    if (hit != m->overriddenHuds.end())
    {
      hud = hit->second;
    }
    else
    {
      hud = injectHud(injector, view);
      m->overriddenHuds[view] = hud;
    }

    setHud(view, hud);
  }

  double AbstractGameState::getSimulationTime() const
  {
    return m->simulationTime;
  }

  double AbstractGameState::getFrameTime() const
  {
    return m->frameTime;
  }

  osg::ref_ptr<World> AbstractGameState::getWorld(const osg::ref_ptr<View>& view) const
  {
    auto it = m->overriddenWorlds.find(view);
    if (it != m->overriddenWorlds.end())
	  {
      return it->second;
	  }

    return nullptr;
  }

  osg::ref_ptr<Hud> AbstractGameState::getHud(const osg::ref_ptr<View>& view) const
  {
    auto it = m->overriddenHuds.find(view);
    return it != m->overriddenHuds.end() ? it->second : nullptr;
  }

  osg::ref_ptr<osgGaming::Viewer> AbstractGameState::getViewer() const
  {
    return m->viewer;
  }

  osg::ref_ptr<osgGaming::View> AbstractGameState::getView(int i) const
  {
    return m->viewer->getView(i);
  }

  osg::ref_ptr<GameSettings> AbstractGameState::getGameSettings() const
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

  void AbstractGameState::setWorld(const osg::ref_ptr<View>& view, const osg::ref_ptr<World>& world)
  {
    m->overriddenWorlds[view] = world;
  }

  void AbstractGameState::setHud(const osg::ref_ptr<View>& view, const osg::ref_ptr<Hud>& hud)
  {
    m->overriddenHuds[view] = hud;
  }

  void AbstractGameState::setViewer(const osg::ref_ptr<Viewer>& viewer)
  {
    m->viewer = viewer;
  }

  void AbstractGameState::setGameSettings(const osg::ref_ptr<GameSettings>& settings)
  {
    m->gameSettings = settings;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_default()
  {
    return m->stateEvent;
  }

  void AbstractGameState::setInjector(Injector& injector)
  {
    m_injector = &injector;
  }

  void AbstractGameState::toAbstractGameStateRefList(AbstractGameStateList& list, AbstractGameStateRefList& refList)
  {
    for (auto& it : list)
    {
      refList.push_back(std::reference_wrapper<osg::ref_ptr<AbstractGameState>>(it));
    }
  }

  osg::ref_ptr<World> AbstractGameState::injectWorld(Injector& injector, const osg::ref_ptr<View>& view)
  {
    return nullptr;
  }

  osg::ref_ptr<Hud> AbstractGameState::injectHud(Injector& injector, const osg::ref_ptr<View>& view)
  {
    return nullptr;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_push(const osg::ref_ptr<AbstractGameState>& state)
  {
    if (m->stateEvent != nullptr)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = StateEventType::PUSH;
    m->stateEvent->referencedStates.push_back(state);

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_pop()
  {
    if (m->stateEvent != nullptr)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = StateEventType::POP;

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_replace(const osg::ref_ptr<AbstractGameState>& state)
  {
    if (m->stateEvent != nullptr)
    {
      return m->stateEvent;
    }

    m->stateEvent       = new StateEvent();
    m->stateEvent->type = StateEventType::REPLACE;
    m->stateEvent->referencedStates.push_back(state);

    return m->stateEvent;
  }

  AbstractGameState::StateEvent* AbstractGameState::stateEvent_endGame()
  {
    if (m->stateEvent != nullptr)
    {
      return m->stateEvent;
    }

    m->stateEvent = new StateEvent();
    m->stateEvent->type = StateEventType::END_GAME;

    return m->stateEvent;
  }

}
