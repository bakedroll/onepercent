#include "scripting/LuaSimulation.h"

#include "core/Macros.h"
#include "core/Multithreading.h"
#include "simulation/Simulation.h"

extern "C"
{
#include <lua.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  LuaSimulation::LuaSimulation(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance("simulation")
    , m_simulation(injector.inject<Simulation>())
    , m_oDay(injector.inject<ODay>())
    , m_oNumSkillPoints(injector.inject<ONumSkillPoints>())
  {
  }

  LuaSimulation::~LuaSimulation() = default;

  void LuaSimulation::lua_start(lua_State* state)
  {
    Multithreading::uiExecuteOrAsync([this](){ m_simulation->start(); });
  }

  void LuaSimulation::lua_stop(lua_State* state)
  {
    Multithreading::uiExecuteOrAsync([this](){ m_simulation->stop(); });
  }

  void LuaSimulation::lua_set_skill_points(int points)
  {
    OSGG_QLOG_DEBUG(QString("Set skill points: %1").arg(points));
    Multithreading::uiExecuteOrAsync([=](){ m_oNumSkillPoints->set(points); });
  }

  void LuaSimulation::lua_add_skill_points(int points)
  {
    OSGG_QLOG_DEBUG(QString("Add skill points: %1").arg(points));
    Multithreading::uiExecuteOrAsync([=]()
    {
      int p = m_oNumSkillPoints->get();
      m_oNumSkillPoints->set(p + points);
    });
  }

  void LuaSimulation::lua_set_day(int day)
  {
    OSGG_QLOG_DEBUG(QString("Set day: %1").arg(day));
    Multithreading::uiExecuteOrAsync([=](){ m_oDay->set(day); });
  }

  void LuaSimulation::lua_set_interval(int interval)
  {
    OSGG_QLOG_DEBUG(QString("Set interval: %1").arg(interval));
    Multithreading::uiExecuteOrAsync([=](){ m_simulation->setUpdateTimerInterval(interval); });
  }

  void LuaSimulation::lua_set_tick_update_mode(int mode)
  {
    m_simulation->setTickUpdateMode(static_cast<Simulation::TickUpdateMode>(mode));
  }

  void LuaSimulation::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<LuaSimulation>("Simulation")
      .addFunction("start", &LuaSimulation::lua_start)
      .addFunction("stop", &LuaSimulation::lua_stop)
      .addFunction("set_day", &LuaSimulation::lua_set_day)
      .addFunction("set_interval", &LuaSimulation::lua_set_interval)
      .addFunction("set_skill_points", &LuaSimulation::lua_set_skill_points)
      .addFunction("add_skill_points", &LuaSimulation::lua_add_skill_points)
      .addFunction("set_tick_update_mode", &LuaSimulation::lua_set_tick_update_mode)
      .endClass();
  }
}