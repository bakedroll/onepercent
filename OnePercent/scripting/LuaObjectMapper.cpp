#include "LuaObjectMapper.h"

#include <QString>

namespace onep
{
  template<typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }

  LuaObjectMapper::LuaObjectMapper(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTable(object, luaState)
  {
  }

  LuaObjectMapper::~LuaObjectMapper() = default;

  void LuaObjectMapper::foreachElementDo(const std::function<void(luabridge::LuaRef& key, luabridge::LuaRef& value)> func)
  {
    for (luabridge::Iterator it(luaRef()); !it.isNil(); ++it)
    {
      luabridge::LuaRef value = it.value();
      luabridge::LuaRef key   = it.key();

      func(key, value);
    }
  }

  int LuaObjectMapper::getNumElements() const
  {
    return static_cast<int>(m_elements.size());
  }
}
