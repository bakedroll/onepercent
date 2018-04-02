#include "Skill.h"

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

  Skill::Skill(std::string name, std::string displayName, std::string type, int cost)
    : osg::Referenced()
    , m(new Impl())
  {
    m->name = name;
    m->displayName = displayName;
    m->type = type;
    m->cost = cost;
  }

  std::string Skill::getName()
  {
    return m->name;
  }

  osgGaming::Observable<bool>::Ptr Skill::getObActivated() const
  {
    return m->obActivated;
  }
}