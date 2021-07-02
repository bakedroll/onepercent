#pragma once

#include <osgHelper/Observable.h>

#include <luaHelper/LuaBridgeDefinition.h>
#include <luaHelper/LuaCallbackRegistry.h>
#include <luaHelper/LuaStateManager.h>
#include <luaHelper/LuaTableMappedObject.h>

#include <LuaBridge/LuaBridge.h>

#include <vector>
#include <memory>

namespace onep
{
	class LuaSkill : public luaHelper::LuaCallbackRegistry, public luaHelper::LuaTableMappedObject
	{
	public:
    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override;
    };

    typedef std::shared_ptr<LuaSkill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<std::string, Ptr> Map;

    LuaSkill(const luabridge::LuaRef& object, lua_State* lua, const luaHelper::LuaStateManager::Ptr& luaStateManager);
    ~LuaSkill();

    void update(const std::string& branchName, luabridge::LuaRef countryState);

		const std::string& getName() const;
    const std::string& getBranchName() const;
    const std::string& getDisplayName() const;
    int                getCost() const;

    bool getIsActivated() const;
    void setIsActivated(bool activated);
    osgHelper::Observable<bool>::Ptr getObActivated() const;

    const std::vector<std::string>& getDependencies() const;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
