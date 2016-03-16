#pragma once

#include "SimulationCallback.h"

#include <osg/ref_ptr>
#include <osg/Node>

#include <vector>

namespace onep
{
	class Skill : public osg::Node, public SimulationCallback
	{
	public:
    typedef osg::ref_ptr<Skill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<int, Ptr> Map;

		Skill(std::string name);

    void addAttribute(CountryValueType valueType, ProgressingValueMethod method, float value);
    void addBranchAttribute(BranchType branch, CountryValueType valueType, ProgressingValueMethod method, float value);

		void setActivated(bool activated);

		std::string getName();
		bool getActivated();

    virtual bool callback(SimulationVisitor* visitor) override;
	  
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

      BranchType branchType;
      Attribute attribute;
    } BranchAttribute;

    Attribute::List m_attributes;
    BranchAttribute::List m_branchAttributes;

	  std::string m_name;
		bool m_activated;
	};
}