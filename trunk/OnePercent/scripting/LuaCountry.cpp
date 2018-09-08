#include "LuaCountry.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct LuaCountry::Impl
  {
    Impl() {}
    ~Impl() {}

    int id;
    std::string name;
    std::vector<int> neighbourIds;
  };

  LuaCountry::LuaCountry(const luabridge::LuaRef& object)
    : LuaObjectMapper(object)
    , m(new Impl())
  {
    assert_return(object.isTable());

    luabridge::LuaRef idRef         = object["id"];
    luabridge::LuaRef nameRef       = object["name"];

    assert_return(idRef.isNumber());
    assert_return(nameRef.isString());

    m->id = idRef;
    m->name = nameRef.tostring();
  }

  LuaCountry::~LuaCountry() = default;

  int LuaCountry::getId() const
  {
    return m->id;
  }

  std::string LuaCountry::getName() const
  {
    return m->name;
  }

  std::vector<int>& LuaCountry::getNeighbourIds() const
  {
    return m->neighbourIds;
  }

  void LuaCountry::onUpdate(luabridge::LuaRef& object)
  {
    luabridge::LuaRef refNeighbours = object["neighbours"];
    int i = 1;
    for (std::vector<int>::const_iterator it = m->neighbourIds.cbegin(); it != m->neighbourIds.cend(); ++it)
      refNeighbours[i++] = *it;
  }
}