#pragma once

#include "scripting/LuaClassDefinition.h"
#include "scripting/LuaCallbackRegistry.h"
#include "scripting/LuaStateManager.h"

#include <osgGaming/Observable.h>

#include <LuaBridge/LuaBridge.h>

#include <vector>
#include <memory>

namespace onep
{
	class LuaSkill : public LuaCallbackRegistry
	{
	public:
    class Definition : public LuaClassDefinition
    {
    public:
      void registerClass(lua_State* state) override;
    };

    typedef std::shared_ptr<LuaSkill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

    LuaSkill(const LuaStateManager::Ptr& lua, const luabridge::LuaRef& object);
    ~LuaSkill();

    void update(const std::string& branchName, luabridge::LuaRef countryState);

		std::string getName() const;
    std::string getBranchName() const;
    std::string getDisplayName() const;

    bool getIsActivated() const;
    void setIsActivated(bool activated);
    osgGaming::Observable<bool>::Ptr getObActivated() const;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}