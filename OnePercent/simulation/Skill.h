#pragma once

#include "scripting/LuaObjectMapper.h"

#include <osg/ref_ptr>
#include <osg/Node>

#include <vector>

#include <osgGaming/Observable.h>

#include <memory>

namespace onep
{
	class Skill : public osg::Referenced, public LuaObjectMapper
	{
	public:
    typedef osg::ref_ptr<Skill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

	  explicit Skill(const luabridge::LuaRef& object);

		std::string getSkillName() const;
    std::string getDisplayName() const;

    osgGaming::Observable<bool>::Ptr getObActivated() const;

  protected:

    virtual void writeObject(luabridge::LuaRef& object) const override;
    virtual void readObject(const luabridge::LuaRef& object) override;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}