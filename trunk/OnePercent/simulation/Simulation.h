#pragma once

#include <osgGaming/Injector.h>
#include <osgGaming/Observable.h>

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

    typedef osg::ref_ptr<Simulation> Ptr;

		Simulation(osgGaming::Injector& injector);
    ~Simulation();

    void prepare();
    void shutdownUpdateThread();

    UpdateThread* getUpdateThread();

    void addSkillPoints(int points);
    bool paySkillPoints(int points);
    bool switchSkillBranchState(int countryId, const std::shared_ptr<LuaSkillBranch>& branch, SkillBranchState state);

    void start();
    void stop();
    bool running() const;
    osgGaming::Observable<bool>::Ptr getORunning() const;

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