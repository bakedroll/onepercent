#include <osgGaming/TextResource.h>

using namespace osgGaming;
using namespace osg;

Object* TextResource::cloneType() const
{
	ref_ptr<TextResource> res = new TextResource();
	res->text = text;
	return res.get();
}

Object* TextResource::clone(const CopyOp& copyOp) const
{
	ref_ptr<TextResource> res = new TextResource();
	res->text = text;
	return res.get();
}

const char* TextResource::libraryName() const
{
	return "osgGaming";
}

const char* TextResource::className() const
{
	return "TextResource";
}