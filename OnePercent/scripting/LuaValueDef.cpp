#include "scripting/LuaValueDef.h"

#include <QString>

namespace onep
{
  struct LuaValueDef::Impl
  {
    Impl()
      : type(Type::Default)
      , init(0.0)
    {}

    std::string name;
    Type        type;
    float       init;
  };

  LuaValueDef::LuaValueDef(const luabridge::LuaRef& object, lua_State* luaState)
    : LuaObjectMapper(object, luaState)
    , m(new Impl())
  {
    checkForConsistency("name", LUA_TSTRING);
    checkForConsistency("type", LUA_TSTRING);
    checkForConsistency("init", LUA_TNUMBER);

    assert_return(!hasAnyInconsistency());

    luabridge::LuaRef nameRef = object["name"];
    luabridge::LuaRef typeRef = object["type"];
    luabridge::LuaRef initRef = object["init"];

    auto typeStr = typeRef.tostring();

    m->name = nameRef.tostring();
    m->init = initRef;

    if (typeStr == "default")
    {
      m->type = Type::Default;
    }
    else if (typeStr == "branch")
    {
      m->type = Type::Branch;
    }
    else
    {
      assert(false);
      OSGG_QLOG_WARN(QString("Unknown value type '%1'").arg(QString::fromStdString(typeStr)));
    }
  }

  LuaValueDef::~LuaValueDef() = default;

  std::string LuaValueDef::getName() const
  {
    return m->name;
  }

  LuaValueDef::Type LuaValueDef::getType() const
  {
    return m->type;
  }

  float LuaValueDef::getInit() const
  {
    return m->init;
  }
}