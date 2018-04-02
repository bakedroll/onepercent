#pragma once

#include <osg/ref_ptr>
#include <osg/Node>

#include <vector>

#include <osgGaming/Observable.h>

#include <memory>

namespace onep
{
	class Skill : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<Skill> Ptr;
    typedef std::vector<Ptr> List;
    typedef std::map<int, Ptr> Map;

    Skill(std::string name, std::string displayName, std::string type, int cost);

		std::string getName();

    osgGaming::Observable<bool>::Ptr getObActivated() const;
	  
  private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}