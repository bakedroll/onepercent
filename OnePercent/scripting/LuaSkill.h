#pragma once

#include "scripting/LuaBridgeDefinition.h"
#include "scripting/LuaCallbackRegistry.h"
#include "scripting/LuaStateManager.h"
#include "scripting/LuaObjectMapper.h"

#include <osgGaming/Observable.h>

#include <LuaBridge/LuaBridge.h>

#include <vector>
#include <memory>

namespace onep
{
	class LuaSkill : public LuaCallbackRegistry, public LuaObjectMapper
	{
	public:
    class Definition : public LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    typedef std::shared_ptr<LuaSkill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

    LuaSkill(const luabridge::LuaRef& object, lua_State* lua, const LuaStateManager::Ptr& luaStateManager);
    ~LuaSkill();

    void update(const std::string& branchName, luabridge::LuaRef countryState);

		const std::string& getName() const;
    const std::string& getBranchName() const;
    const std::string& getDisplayName() const;
    int                getCost() const;

    bool getIsActivated() const;
    void setIsActivated(bool activated);
    osgGaming::Observable<bool>::Ptr getObActivated() const;

    const std::vector<std::string>& getDependencies() const;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
