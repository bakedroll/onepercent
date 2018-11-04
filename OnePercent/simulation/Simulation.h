#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/Observable.h>

namespace onep
{
  class LuaSimulationStateTable;
  class UpdateThread;

	class Simulation : public osg::Referenced
	{
	public:
    enum class TickUpdateMode
    {
      LUA,
      CPP
    };

    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgGaming::Injector& injector);
    ~Simulation();

    void prepare();
    void shutdownUpdateThread();

    UpdateThread* getUpdateThread();

    bool paySkillPoints(int points);

    void start();
    void stop();
    bool running() const;
    osgGaming::Observable<bool>::Ptr getORunning() const;

    void setUpdateTimerInterval(int msecs);
    void setTickUpdateMode(TickUpdateMode mode);

    void saveState(const std::string& filename);
    void loadState(const std::string& filename);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}