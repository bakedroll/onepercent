#pragma once

#include <osgGaming/World.h>
#include <osgGaming/GameSettings.h>
#include <osgGaming/GameState.h>
#include <osgGaming/InputManager.h>
#include <osgGaming/Viewer.h>
#include <osgGaming/SimulationCallback.h>

#include <vector>
#include <future>

namespace osgGaming
{
	typedef std::vector<osg::ref_ptr<GameState>> GameStateList;

	class GameApplication : public SimulationCallback
	{
	public:
		GameApplication();

		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) override;

		template<class WorldType>
		void setWorld() { setWorld(new WorldType()); }

		template<class WorldLoadingType>
		void setWorldLoading() { setWorldLoading(new WorldLoadingType()); }

		template<class SettingsType>
		void setGameSettings() { setGameSettings(new SettingsType()); }

		osg::ref_ptr<GameSettings> getGameSettings();

		int run(osg::ref_ptr<GameState> initialState);

	private:
		void setWorld(osg::ref_ptr<World> world);
		void setWorldLoading(osg::ref_ptr<World> world);
		void setGameSettings(osg::ref_ptr<GameSettings> settings);

		void attachWorld(osg::ref_ptr<World> world);

		void popState();
		void pushState(osg::ref_ptr<GameState> state);
		void replaceState(osg::ref_ptr<GameState> state);

		GameStateList _stateStack;

		Viewer _viewer;

		osg::ref_ptr<InputManager> _inputManager;

		osg::ref_ptr<World> _world;
		osg::ref_ptr<World> _worldLoading;
		osg::ref_ptr<GameSettings> _gameSettings;

		bool _gameEnded;
		bool _isLoading;

		std::future<void> _loadingThreadFuture;
	};
}