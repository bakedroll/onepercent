#include "LuaObjectMapper.h"

namespace onep
{
  LuaObjectMapper::LuaObjectMapper(const luabridge::LuaRef& object)
    : m_ref(std::make_unique<luabridge::LuaRef>(object))
  {
  }

  LuaObjectMapper::~LuaObjectMapper()
  {
  }

  void LuaObjectMapper::write()
  {
    writeObject(*m_ref.get());
  }

  void LuaObjectMapper::read()
  {
    readObject(*m_ref.get());
  }
}