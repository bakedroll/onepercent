#pragma once

#include <osg/Vec3>
#include <osg/Matrix>

using namespace osg;

#define C_PI 3.14159265359

namespace onep
{
	void rotateVector(Vec3* vec, Quat quat);
	void transformVector(Vec3* vec, Matrixd* mat);

	Quat getQuatFromEuler(double pitch, double roll, double yaw);

	void generateTangentAndBinormal(Node* node);
}