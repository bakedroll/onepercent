#include "Skill.h"

#include "core/Macros.h"

namespace onep
{
  struct Skill::Impl
  {
    Impl()
      : obActivated(new osgGaming::Observable<bool>(false))
    {}

    std::string name;
    std::string displayName;
    std::string type;
    int cost;

    osgGaming::Observable<bool>::Ptr obActivated;
  };

  Skill::Skill(const luabridge::LuaRef& object)
    : osg::Referenced()
    , LuaObjectMapper(object)
    , m(new Impl())
  {
    luabridge::LuaRef nameRef         = object["name"];
    luabridge::LuaRef displayNameRef  = object["display_name"];
    luabridge::LuaRef typeRef         = object["type"];
    luabridge::LuaRef costRef         = object["cost"];
    luabridge::LuaRef activatedRef    = object["activated"];

    assert_return(nameRef.isString());
    assert_return(displayNameRef.isString());
    assert_return(typeRef.isString());
    assert_return(costRef.isNumber());
    assert_return(activatedRef.type() == LUA_TBOOLEAN);

    m->name         = nameRef.tostring();
    m->displayName  = displayNameRef.tostring();
    m->type         = typeRef.tostring();
    m->cost         = costRef;
    m->obActivated->set(bool(activatedRef));
  }

  std::string Skill::getSkillName() const
  {
    return m->name;
  }

  std::string Skill::getDisplayName() const
  {
    return m->displayName;
  }

  osgGaming::Observable<bool>::Ptr Skill::getObActivated() const
  {
    return m->obActivated;
  }

  void Skill::writeObject(luabridge::LuaRef& object) const
  {
    object["activated"] = m->obActivated->get();
  }

  void Skill::readObject(const luabridge::LuaRef& object)
  {
  }
}