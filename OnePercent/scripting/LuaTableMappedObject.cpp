#include "LuaTableMappedObject.h"

#include <QString>

namespace onep
{
  template<typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args)
  {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }

  LuaTableMappedObject::LuaTableMappedObject(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaTable(object, luaState)
  {
  }

  LuaTableMappedObject::~LuaTableMappedObject() = default;

  int LuaTableMappedObject::getNumMappedObjects() const
  {
    return static_cast<int>(m_mappedObjects.size());
  }
}
