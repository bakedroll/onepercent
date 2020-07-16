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

  bool LuaObjectMapper::hasAnyInconsistency() const
  {
    return !m_inconsistencies.empty();
  }

  void LuaObjectMapper::logInconsistencies(const std::string& ownKey) const
  {
    auto message = QString("The following inconsistencies were detected for lua object \"%1\":\n").arg(ownKey.c_str());
    for (const auto& inconsistency : m_inconsistencies)
    {
      message +=
              QString("  \"%1\": %2\n")
                      .arg(inconsistency.key.c_str())
                      .arg(inconsistency.type == InconsistencyType::ElementMissing ? "Element missing" : "Wrong type");
    }

    OSGG_QLOG_WARN(message);
  }

  bool LuaObjectMapper::checkForConsistency(const std::string& key, int luaType)
  {
    luabridge::LuaRef ref            = (luaRef())[key];
    auto              elementMissing = ref.isNil();

    if (elementMissing || (ref.type() != luaType))
    {
      m_inconsistencies.push_back(
              {key, elementMissing ? InconsistencyType::ElementMissing : InconsistencyType::WrongType});
      return false;
    }

    return true;
  }
}
