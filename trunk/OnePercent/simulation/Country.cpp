#include "Country.h"

#include "core/Macros.h"

namespace onep
{
  struct Country::Impl
  {
    Impl() {}

    int id;
    std::string name;
    float population;
    float wealth;

    std::vector<int> neighbourIds;
  };

  Country::Country(const luabridge::LuaRef& object)
    : osg::Referenced()
    , LuaObjectMapper(object)
    , m(new Impl())
  {
    luabridge::LuaRef idRef         = object["id"];
    luabridge::LuaRef nameRef       = object["name"];
    luabridge::LuaRef populationRef = object["population"];
    luabridge::LuaRef wealthRef     = object["wealth"];

    assert_return(idRef.isNumber());
    assert_return(nameRef.isString());
    assert_return(populationRef.isNumber());
    assert_return(wealthRef.isNumber());

    m->id = idRef;
    m->name = nameRef.tostring();
    m->population = populationRef;
    m->wealth = wealthRef;
  }

  Country::~Country()
  {
  }

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

  void Country::writeObject(luabridge::LuaRef& object) const
  {
    luabridge::LuaRef refNeighbours = object["neighbours"];
    int i = 1;
    for (std::vector<int>::const_iterator it = m->neighbourIds.cbegin(); it != m->neighbourIds.cend(); ++it)
      refNeighbours[i++] = *it;
  }

  void Country::readObject(const luabridge::LuaRef& object)
  {
  }
}