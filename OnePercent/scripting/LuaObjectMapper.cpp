#include "LuaObjectMapper.h"

namespace onep
{
  template<typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args) {
      return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }

  LuaObjectMapper::LuaObjectMapper(const luabridge::LuaRef& object)
    : m_ref(make_unique<luabridge::LuaRef>(object))
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