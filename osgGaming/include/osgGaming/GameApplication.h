#pragma once

#include <osgGaming/SimulationCallback.h>
#include <osgGaming/GameStateStack.h>

#include "Viewer.h"

namespace osgGaming
{
  class Signal;
  class World;
  class Hud;
  class InputManager;
  class AbstractGameState;
  class GameSettings;
  class View;

  class GameApplication : public SimulationCallback
	{
	public:
    GameApplication();
    ~GameApplication();

		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

		osg::ref_ptr<World> getDefaultWorld();
		osg::ref_ptr<Hud> getDefaultHud();
		osg::ref_ptr<GameSettings> getDefaultGameSettings();

		void setDefaultWorld(osg::ref_ptr<World> world);
		void setDefaultHud(osg::ref_ptr<Hud> hud);
    void setDefaultGameSettings(osg::ref_ptr<GameSettings> settings);

		int run(osg::ref_ptr<AbstractGameState> initialState);
		int run(GameStateStack::AbstractGameStateList initialStates);

    osgGaming::Signal& onEndGameSignal();
    osg::ref_ptr<osgGaming::Viewer> getViewer();

  protected:
    virtual int mainloop() = 0;
    virtual osg::ref_ptr<InputManager> createInputManager(osg::ref_ptr<osgGaming::View> view) = 0;

    virtual void stateAttachedEvent(osg::ref_ptr<AbstractGameState> state);

    bool isGameEnded() const;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
