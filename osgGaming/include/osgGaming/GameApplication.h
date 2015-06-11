#pragma once

#include <osgGaming/World.h>
#include <osgGaming/Hud.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/AbstractGameState.h>
#include <osgGaming/InputManager.h>
#include <osgGaming/Viewer.h>
#include <osgGaming/SimulationCallback.h>

#include <vector>
#include <future>

namespace osgGaming
{
	typedef std::vector<osg::ref_ptr<AbstractGameState>> GameStateList;

	class GameApplication : public SimulationCallback
	{
	public:
		GameApplication();

		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

		osg::ref_ptr<World> getDefaultWorld();
		osg::ref_ptr<Hud> getDefaultHud();
		osg::ref_ptr<GameSettings> getDefaultGameSettings();

		void setDefaultWorld(osg::ref_ptr<World> world);
		void setDefaultHud(osg::ref_ptr<Hud> hud);
		void setDefaultGameSettings(osg::ref_ptr<GameSettings> settings);

		int run(osg::ref_ptr<AbstractGameState> initialState);

	private:
		void initializeState(osg::ref_ptr<AbstractGameState> state);
		void prepareStateWorldAndHud(osg::ref_ptr<AbstractGameState> state);
		void attachState(osg::ref_ptr<AbstractGameState> state);

		void popState();
		void pushState(osg::ref_ptr<AbstractGameState> state);
		void replaceState(osg::ref_ptr<AbstractGameState> state);

		GameStateList _stateStack;

		Viewer _viewer;

		osg::ref_ptr<InputManager> _inputManager;

		osg::ref_ptr<World> _defaultWorld;
		osg::ref_ptr<Hud> _defaultHud;
		osg::ref_ptr<GameSettings> _defaultGameSettings;

		bool _gameEnded;
		bool _isLoading;
		bool _attachState;

		std::future<void> _loadingThreadFuture;
	};
}