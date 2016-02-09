#include <osgGaming/BinaryResource.h>

using namespace osgGaming;
using namespace osg;

BinaryResource::BinaryResource()
	: Object(),
	  _bytes(nullptr)
{

}

BinaryResource::~BinaryResource()
{
	if (_bytes != nullptr)
	{
		delete[] _bytes;
	}
}

Object* BinaryResource::cloneType() const
{
	ref_ptr<BinaryResource> res = new BinaryResource();
	return res.get();
}

Object* BinaryResource::clone(const CopyOp& copyOp) const
{
	ref_ptr<BinaryResource> res = new BinaryResource();
	return res.get();
}

const char* BinaryResource::libraryName() const
{
	return "osgGaming";
}

const char* BinaryResource::className() const
{
	return "BinaryResource";
}

char* BinaryResource::getBytes()
{
	return _bytes;
}

void BinaryResource::setBytes(char* bytes)
{
	if (_bytes != nullptr)
	{
		delete[] _bytes;
	}

	_bytes = bytes;
}