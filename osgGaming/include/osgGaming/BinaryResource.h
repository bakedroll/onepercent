#pragma once

#include <osg/Object>

namespace osgGaming
{
	class BinaryResource : public osg::Object
	{
	public:
		BinaryResource();
		~BinaryResource();

		virtual osg::Object* cloneType() const override;
		virtual osg::Object* clone(const osg::CopyOp& copyOp) const override;
		virtual const char* libraryName() const override;
		virtual const char* className() const override;

		char* getBytes();
		void setBytes(char* bytes);
		
	private:
		char* _bytes;
	};
}