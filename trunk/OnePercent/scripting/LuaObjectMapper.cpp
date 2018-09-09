#include "LuaObjectMapper.h"

namespace onep
{
  template<typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }

  LuaObjectMapper::LuaObjectMapper(const luabridge::LuaRef& object, lua_State* luaState)
    : m_ref(make_unique<luabridge::LuaRef>(object))
    , m_luaState(luaState)
  {
  }

  LuaObjectMapper::~LuaObjectMapper() = default;

  luabridge::LuaRef& LuaObjectMapper::luaref() const
  {
    return *m_ref;
  }

  void LuaObjectMapper::foreachElementDo(std::function<void(luabridge::LuaRef& key, luabridge::LuaRef& value)> func)
  {
    for (luabridge::Iterator it(*m_ref); !it.isNil(); ++it)
    {
      luabridge::LuaRef value = *it;
      luabridge::LuaRef key = it.key();

      func(key, value);
    }
  }

  void LuaObjectMapper::traverseElements(int type)
  {
    onTraverse(type, *m_ref);
    for (auto& elem : m_elements)
    {
      auto luaObject = elem.second.get();

      //luaObject->onTraverse(type, *luaObject->m_ref);
      luaObject->traverseElements(type);
    }
  }

  int LuaObjectMapper::getNumElements() const
  {
    return int(m_elements.size());
  }

  void LuaObjectMapper::onTraverse(int type, luabridge::LuaRef& object)
  {
  }
}