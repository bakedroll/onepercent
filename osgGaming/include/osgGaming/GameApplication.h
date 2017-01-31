#pragma once

#include <osgGaming/World.h>
#include <osgGaming/Hud.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/AbstractGameState.h>
#include <osgGaming/InputManager.h>
#include <osgGaming/SimulationCallback.h>
#include <osgGaming/GameStateStack.h>

#include <future>
#include "Viewer.h"

namespace osgGaming
{
	class GameApplication : public SimulationCallback
	{
	public:
    GameApplication(osg::ref_ptr<osgGaming::View> view = nullptr);

		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

		osg::ref_ptr<World> getDefaultWorld();
		osg::ref_ptr<Hud> getDefaultHud();
		osg::ref_ptr<GameSettings> getDefaultGameSettings();

		void setDefaultWorld(osg::ref_ptr<World> world);
		void setDefaultHud(osg::ref_ptr<Hud> hud);
    void setDefaultGameSettings(osg::ref_ptr<GameSettings> settings);

		int run(osg::ref_ptr<AbstractGameState> initialState);
		int run(GameStateStack::AbstractGameStateList initialStates);

    osg::ref_ptr<osgGaming::Viewer> getViewer();
    osg::ref_ptr<osgGaming::View> getView();

  protected:
    virtual int mainloop();
    virtual osg::ref_ptr<InputManager> obtainInputManager(osg::ref_ptr<osgGaming::View> view);

    bool isGameRunning() const;

	private:
		void initializeState(osg::ref_ptr<AbstractGameState> state);
		void prepareStateWorldAndHud(osg::ref_ptr<AbstractGameState> state);
		void prepareStateWorldAndHud(AbstractGameState::AbstractGameStateList states);
		void attachState(osg::ref_ptr<AbstractGameState> state);

		GameStateStack m_gameStateStack;

    osg::ref_ptr<osgGaming::Viewer> m_viewer;
    osg::ref_ptr<osgGaming::View> m_view;

		osg::ref_ptr<InputManager> m_inputManager;

		osg::ref_ptr<World> m_defaultWorld;
		osg::ref_ptr<Hud> m_defaultHud;
		osg::ref_ptr<GameSettings> m_defaultGameSettings;

		bool m_gameEnded;
		bool m_isLoading;

		std::future<void> m_loadingThreadFuture;
	};
}