#pragma once

#include <osgHelper/ioc/Injector.h>
#include <osgHelper/Observable.h>

#include "scripting/LuaDefines.h"

namespace onep
{
  class LuaSimulationStateTable;
  class LuaSkillBranch;
  class UpdateThread;

	class Simulation : public osg::Referenced
	{
	public:
    enum class SkillBranchState
    {
      PURCHASED,
      RESIGNED
    };

    enum class State : int
    {
      Paused,
      NormalSpeed     = 100,
      FastForward = 50
    };

    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgHelper::ioc::Injector& injector);
    virtual ~Simulation();

    void prepare();
    void shutdownUpdateThread();

    UpdateThread* getUpdateThread();

    void addSkillPoints(int points);
    bool paySkillPoints(int points);
    bool switchSkillBranchState(int countryId, const std::shared_ptr<LuaSkillBranch>& branch, SkillBranchState state);

    void setState(State state);
    void setAutoPause(bool enabled);

    void start();
    void stop();

    bool isRunning() const;
    bool isAutoPauseEnabled() const;

    osgHelper::Observable<State>::Ptr getOState() const;

    void setUpdateTimerInterval(int msecs);
    void setProfilingLogsEnabled(bool enabled);
    void setTickUpdateMode(LuaDefines::TickUpdateMode mode);

    void saveState(const std::string& filename);
    void loadState(const std::string& filename);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}