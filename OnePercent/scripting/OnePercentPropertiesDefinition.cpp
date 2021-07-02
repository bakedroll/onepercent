#include "scripting/OnePercentPropertiesDefinition.h"

#include "scripting/LuaConfig.h"
#include "scripting/LuaControl.h"
#include "scripting/LuaSimulation.h"
#include "scripting/LuaVisuals.h"
#include "nodes/CountryOverlay.h"

#include <luaHelper/LuaLogger.h>

namespace onep
{
  OnePercentPropertiesDefinition::OnePercentPropertiesDefinition(osgHelper::ioc::Injector& injector)
    : LuaPropertiesDefinition(injector)
  {
  }

  OnePercentPropertiesDefinition::~OnePercentPropertiesDefinition() = default;

  void OnePercentPropertiesDefinition::registerDefinition(lua_State* state)
  {
    addProperty<LuaConfig>(state, "config");
    addProperty<LuaControl>(state, "control");
    addProperty<LuaSimulation>(state, "simulation");
    addProperty<LuaVisuals>(state, "visuals");
    addProperty<luaHelper::LuaLogger>(state, "log");
    addProperty<CountryOverlay>(state, "countries");
  }
}
