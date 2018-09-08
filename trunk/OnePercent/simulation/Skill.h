#pragma once

#include "scripting/LuaObjectMapper.h"

#include <osgGaming/Observable.h>

#include <vector>
#include <memory>

namespace onep
{
	class Skill : public LuaObjectMapper
	{
	public:
    typedef std::shared_ptr<Skill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

	  explicit Skill(const luabridge::LuaRef& object);
    ~Skill();

		std::string getSkillName() const;
    std::string getDisplayName() const;

    bool getIsActivated() const;
    void setIsActivated(bool activated);
    osgGaming::Observable<bool>::Ptr getObActivated() const;

  protected:
    virtual void onUpdate(luabridge::LuaRef& object) override;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}