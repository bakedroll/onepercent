#include "scripting/LuaPropertyDefinitions.h"

#include "scripting/LuaConfig.h"
#include "scripting/LuaControl.h"
#include "scripting/LuaSimulation.h"
#include "scripting/LuaVisuals.h"
#include "scripting/LuaLogger.h"
#include "nodes/CountryOverlay.h"

namespace onep
{
  LuaPropertyDefinitions::LuaPropertyDefinitions(osgHelper::ioc::Injector& injector)
    : m_injector(injector)
  {
  }

  LuaPropertyDefinitions::~LuaPropertyDefinitions() = default;

  void LuaPropertyDefinitions::registerDefinition(lua_State* state)
  {
    addProperty<LuaConfig>(state, "config");
    addProperty<LuaControl>(state, "control");
    addProperty<LuaSimulation>(state, "simulation");
    addProperty<LuaVisuals>(state, "visuals");
    addProperty<LuaLogger>(state, "log");
    addProperty<CountryOverlay>(state, "countries");
  }
}
