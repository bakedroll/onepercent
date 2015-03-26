#pragma once

#include <osg/Vec3>
#include <osg/Matrix>

using namespace osg;

#define C_PI 3.14159265359f

namespace onep
{
	void rotateVector(Vec3* vec, Quat quat);
	void transformVector(Vec3* vec, Matrixd* mat);

	Quat getQuatFromEuler(double pitch, double roll, double yaw);
	Vec3 getVec3FromEuler(double pitch, double roll, double yaw, Vec3 origin = Vec3(0.0, 1.0, 0.0));

	void generateTangentAndBinormal(Node* node);
}