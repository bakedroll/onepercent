#pragma once

#include "scripting/LuaObjectMapper.h"

#include <osgGaming/Observable.h>

#include <vector>
#include <memory>

namespace onep
{
	class LuaSkill : public LuaObjectMapper
	{
	public:
    typedef std::shared_ptr<LuaSkill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

	  explicit LuaSkill(const luabridge::LuaRef& object);
    ~LuaSkill();

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