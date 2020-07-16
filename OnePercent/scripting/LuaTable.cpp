#include "scripting/LuaTable.h"
#include "scripting/LuaInvalidDataException.h"

namespace onep
{

LuaTable::LuaTable(const luabridge::LuaRef& object, lua_State* luaState)
  : m_ref(std::make_unique<luabridge::LuaRef>(object))
  , m_luaState(luaState)
{
}

LuaTable::~LuaTable() = default;

luabridge::LuaRef& LuaTable::luaRef() const
{
  assert(m_ref);
  return *m_ref;
}

bool LuaTable::getBoolean(const std::string& key) const
{
  return checkType(getValueRef(key), LUA_TBOOLEAN, key);
}

double LuaTable::getNumber(const std::string& key) const
{
  return checkType(getValueRef(key), LUA_TNUMBER, key);
}

std::string LuaTable::getString(const std::string& key) const
{
  return checkType(getValueRef(key), LUA_TSTRING, key).tostring();
}

luabridge::LuaRef LuaTable::getTable(const std::string& key) const
{
  return checkType(getValueRef(key), LUA_TTABLE, key);
}

luabridge::LuaRef LuaTable::getFunction(const std::string& key) const
{
  return checkType(getValueRef(key), LUA_TFUNCTION, key);
}

luabridge::LuaRef LuaTable::getUserData(const std::string& key) const
{
  return checkType(getValueRef(key), LUA_TUSERDATA, key);
}

lua_State* LuaTable::luaState() const
{
  return m_luaState;
}

luabridge::LuaRef LuaTable::getValueRef(const std::string& key) const
{
  luabridge::LuaRef value = luaRef()[key];
  if (value.isNil())
  {
    throw LuaInvalidDataException(QString("Expected key %1").arg(key.c_str()));
  }

  return value;
}

const luabridge::LuaRef& LuaTable::checkType(const luabridge::LuaRef& ref, int type, const std::string& key) const
{
  if (ref.type() != type)
  {
    throw LuaInvalidDataException(QString("Value %1 has wrong type.").arg(key.c_str()));
  }

  return ref;
}

}
