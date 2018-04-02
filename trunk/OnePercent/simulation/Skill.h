#pragma once

#include "SimulationCallback.h"

#include <osg/ref_ptr>
#include <osg/Node>

#include <vector>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
	class Skill : public osg::Node, public SimulationCallback
	{
	public:
    typedef osg::ref_ptr<Skill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<int, Ptr> Map;

		Skill(std::string name);
    Skill(std::string name, std::string displayName, std::string type, int cost);

    void addAttribute(CountryValueType valueType, ProgressingValueMethod method, float value);
    void addBranchAttribute(int branchId, CountryValueType valueType, ProgressingValueMethod method, float value);

		std::string getName();

    osgGaming::Observable<bool>::Ptr getObActivated() const;

    virtual bool callback(SimulationVisitor* visitor) override;

    void onAction();
    void addOnAction(luabridge::LuaRef& luaRef);
	  
  private:
    typedef struct _attribute
    {
      typedef std::vector<_attribute> List;

      CountryValueType valueType;
      ProgressingValueMethod method;
      float value;
    } Attribute;

    typedef struct _branchAttribute
    {
      typedef std::vector<_branchAttribute> List;

      int branchId;
      Attribute attribute;
    } BranchAttribute;

    Attribute::List m_attributes;
    BranchAttribute::List m_branchAttributes;

    struct Impl;
    std::unique_ptr<Impl> m;
	};
}