#pragma once

#include "scripting/LuaTableMappedObject.h"

namespace onep
{
  class LuaValueGroupTable : public LuaTableMappedObject
  {
  public:
    using ValuesMap = std::map<std::string, float>;
    using Ptr       = std::shared_ptr<LuaValueGroupTable>;
    using GroupsMap = std::map<std::string, Ptr>;

    explicit LuaValueGroupTable(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaValueGroupTable();

    ValuesMap getMap();

    float getValue(const std::string& name) const;
    void  setValue(const std::string&, float value);

    std::shared_ptr<LuaValueGroupTable> getGroup(const std::string& name);
    const GroupsMap& getAllGroups() const;
    

  private:
      std::map<std::string, Ptr> m_groups;

  };
}