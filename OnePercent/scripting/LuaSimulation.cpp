#include "scripting/LuaSimulation.h"

#include "simulation/Simulation.h"

#include <QtUtilsLib/MultithreadedApplication.h>

#include <QtOsgBridge/Macros.h>

extern "C"
{
#include <lua.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  void LuaSimulation::Definition::registerDefinition(lua_State* state)
  {
    getGlobalNamespace(state)
      .beginClass<LuaSimulation>("Simulation")
      .addFunction("start", &LuaSimulation::lua_start)
      .addFunction("stop", &LuaSimulation::lua_stop)
      .addFunction("set_day", &LuaSimulation::lua_set_day)
      .addFunction("set_interval", &LuaSimulation::lua_set_interval)
      .addFunction("set_skill_points", &LuaSimulation::lua_set_skill_points)
      .addFunction("add_skill_points", &LuaSimulation::lua_add_skill_points)
      .addFunction("set_tick_update_mode", &LuaSimulation::lua_set_tick_update_mode)
      .addFunction("set_profiling_logs_enabled", &LuaSimulation::lua_set_profiling_logs_enabled)
      .endClass();
  }

  LuaSimulation::LuaSimulation(osgHelper::ioc::Injector& injector)
    : osg::Referenced()
    , m_simulation(injector.inject<Simulation>())
    , m_oDay(injector.inject<ODay>())
    , m_oNumSkillPoints(injector.inject<ONumSkillPoints>())
  {
  }

  LuaSimulation::~LuaSimulation() = default;

  void LuaSimulation::lua_start(lua_State* state)
  {
    QtUtilsLib::MultithreadedApplication::executeInUiAsync([this](){ m_simulation->start(); });
  }

  void LuaSimulation::lua_stop(lua_State* state)
  {
    QtUtilsLib::MultithreadedApplication::executeInUiAsync([this](){ m_simulation->stop(); });
  }

  void LuaSimulation::lua_set_skill_points(int points)
  {
    UTILS_QLOG_DEBUG(QString("Set skill points: %1").arg(points));
    QtUtilsLib::MultithreadedApplication::executeInUiAsync([=](){ m_oNumSkillPoints->set(points); });
  }

  void LuaSimulation::lua_add_skill_points(int points)
  {
    UTILS_QLOG_DEBUG(QString("Add skill points: %1").arg(points));
    QtUtilsLib::MultithreadedApplication::executeInUiAsync([=]() { m_simulation->addSkillPoints(points); });
  }

  void LuaSimulation::lua_set_day(int day)
  {
    UTILS_QLOG_DEBUG(QString("Set day: %1").arg(day));
    QtUtilsLib::MultithreadedApplication::executeInUiAsync([=](){ m_oDay->set(day); });
  }

  void LuaSimulation::lua_set_interval(int interval)
  {
    UTILS_QLOG_DEBUG(QString("Set interval: %1").arg(interval));
    QtUtilsLib::MultithreadedApplication::executeInUiAsync([=](){ m_simulation->setUpdateTimerInterval(interval); });
  }

  void LuaSimulation::lua_set_tick_update_mode(int mode)
  {
    m_simulation->setTickUpdateMode(static_cast<LuaDefines::TickUpdateMode>(mode));
  }

void LuaSimulation::lua_set_profiling_logs_enabled(bool enabled)
{
    m_simulation->setProfilingLogsEnabled(enabled);
}
}
