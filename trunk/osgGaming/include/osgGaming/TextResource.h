#pragma once

#include <string>

#include <osg/Object>

namespace osgGaming
{
	class TextResource : public osg::Object
	{
	public:
		virtual osg::Object* cloneType() const override;
		virtual osg::Object* clone(const osg::CopyOp& copyOp) const override;
		virtual const char* libraryName() const override;
		virtual const char* className() const override;

		std::string text;
	};
}