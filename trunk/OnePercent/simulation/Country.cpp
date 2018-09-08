#include "Country.h"

#include <osgGaming/Macros.h>

namespace onep
{
  struct Country::Impl
  {
    Impl() {}
    ~Impl() {}

    int id;
    std::string name;
    std::vector<int> neighbourIds;
  };

  Country::Country(const luabridge::LuaRef& object)
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

  Country::~Country() = default;

  int Country::getId() const
  {
    return m->id;
  }

  std::string Country::getName() const
  {
    return m->name;
  }

  std::vector<int>& Country::getNeighbourIds() const
  {
    return m->neighbourIds;
  }

  void Country::onUpdate(luabridge::LuaRef& object)
  {
    luabridge::LuaRef refNeighbours = object["neighbours"];
    int i = 1;
    for (std::vector<int>::const_iterator it = m->neighbourIds.cbegin(); it != m->neighbourIds.cend(); ++it)
      refNeighbours[i++] = *it;
  }
}