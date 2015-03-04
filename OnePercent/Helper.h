#pragma once

#include <osg/Vec3>
#include <osg/Matrix>

using namespace osg;

namespace onep
{
	void transformVector(Vec3* vec, Matrixd* mat);
}