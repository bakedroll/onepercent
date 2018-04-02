#include "SimulatedLuaValue.h"

namespace onep
{
  SimulatedLuaValue::SimulatedLuaValue()
    : SimulatedValue<float>(0.0f, 0.0f, 100.0f)
    , LuaClass()
  {
  }

  SimulatedLuaValue::SimulatedLuaValue(const float& initial)
    : SimulatedValue<float>(initial, 0.0f, 100.0f)
    , LuaClass()
  {

  }

  SimulatedLuaValue::~SimulatedLuaValue()
  {
  }

  void SimulatedLuaValue::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<SimulatedLuaValue>("SimulatedLuaValue")
      .addFunction("set", &SimulatedLuaValue::lua_set)
      .addFunction("get", &SimulatedLuaValue::lua_get)
      .endClass();
  }

  const float& SimulatedLuaValue::lua_get() const
  {
    return SimulatedValue<float>::get();
  }

  void SimulatedLuaValue::lua_set(const float& value)
  {
    SimulatedValue<float>::set(value);
  }

}