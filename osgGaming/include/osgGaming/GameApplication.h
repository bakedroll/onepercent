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

  class GameApplication : public SimulationCallback
	{
	public:
    GameApplication(osg::ref_ptr<osgGaming::View> view = nullptr);
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
    osg::ref_ptr<osgGaming::View> getView();

  protected:
    virtual int mainloop();
    virtual osg::ref_ptr<InputManager> obtainInputManager(osg::ref_ptr<osgGaming::View> view);

    bool isGameRunning() const;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}