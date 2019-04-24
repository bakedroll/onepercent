#include "scripting/LuaPropertyDefinitions.h"

#include "scripting/LuaConfig.h"
#include "scripting/LuaStaticProperty.h"
#include "LuaControl.h"
#include "LuaSimulation.h"
#include "LuaVisuals.h"
#include "LuaLogger.h"
#include "nodes/CountryOverlay.h"

namespace onep
{
  struct LuaPropertyDefinitions::Impl
  {
    Impl(osgGaming::Injector& injector)
      : config(injector.inject<LuaConfig>())
      , control(injector.inject<LuaControl>())
      , simulation(injector.inject<LuaSimulation>())
      , visuals(injector.inject<LuaVisuals>())
      , logger(injector.inject<LuaLogger>())
      , countryOverlay(injector.inject<CountryOverlay>())
    {}

    LuaConfig::Ptr      config;
    LuaControl::Ptr     control;
    LuaSimulation::Ptr  simulation;
    LuaVisuals::Ptr     visuals;
    LuaLogger::Ptr      logger;
    CountryOverlay::Ptr countryOverlay;
  };

  LuaPropertyDefinitions::LuaPropertyDefinitions(osgGaming::Injector& injector)
    : m(new Impl(injector))
  {
  }

  LuaPropertyDefinitions::~LuaPropertyDefinitions() = default;

  void LuaPropertyDefinitions::registerDefinition(lua_State* state)
  {
    LuaStaticProperty<LuaConfig>::set(m->config.get());
    LuaStaticProperty<LuaControl>::set(m->control.get());
    LuaStaticProperty<LuaSimulation>::set(m->simulation.get());
    LuaStaticProperty<LuaVisuals>::set(m->visuals.get());
    LuaStaticProperty<LuaLogger>::set(m->logger.get());
    LuaStaticProperty<CountryOverlay>::set(m->countryOverlay.get());

    getGlobalNamespace(state)
      .beginNamespace("lua")
      .addProperty("config", LuaStaticProperty<LuaConfig>::get)
      .addProperty("control", LuaStaticProperty<LuaControl>::get)
      .addProperty("simulation", LuaStaticProperty<LuaSimulation>::get)
      .addProperty("visuals", LuaStaticProperty<LuaVisuals>::get)
      .addProperty("log", LuaStaticProperty<LuaLogger>::get)
      .addProperty("countries", LuaStaticProperty<CountryOverlay>::get)
      .endNamespace();
  }
}
