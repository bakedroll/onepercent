#pragma once

#include "scripting/LuaClass.h"

#include <osgGaming/Observable.h>

#include <LuaBridge/LuaBridge.h>

#include <vector>
#include <memory>

namespace onep
{
	class LuaSkill : public LuaClass
	{
	public:
    typedef std::shared_ptr<LuaSkill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

    LuaSkill();
    explicit LuaSkill(const luabridge::LuaRef& object);
    ~LuaSkill();

		std::string getName() const;
    std::string getBranchName() const;
    std::string getDisplayName() const;

    bool getIsActivated() const;
    void setIsActivated(bool activated);
    osgGaming::Observable<bool>::Ptr getObActivated() const;

    void registerClass(lua_State* state) override;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}