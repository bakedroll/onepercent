#pragma once

#include <osg/Vec3>
#include <osg/Matrix>

#include <osg/StateAttribute>

#define C_PI 3.14159265359f

namespace osgGaming
{
	void rotateVector(osg::Vec3* vec, osg::Quat quat);
	void transformVector(osg::Vec3* vec, osg::Matrixd* mat);

	osg::Quat getQuatFromEuler(double pitch, double roll, double yaw);
	osg::Vec3 getVec3FromEuler(double pitch, double roll, double yaw, osg::Vec3 origin = osg::Vec3(0.0, 1.0, 0.0));

	bool sphereLineIntersection(osg::Vec3f sphereCenter, float sphereRadius, osg::Vec3f lineOrigin, osg::Vec3f lineDirectionNornalized, osg::Vec3f& result);

	void generateTangentAndBinormal(osg::Node* node);


	osg::StateAttribute::GLModeValue glModeValueFromBool(bool on);
}