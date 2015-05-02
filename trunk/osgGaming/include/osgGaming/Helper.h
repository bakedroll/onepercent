#pragma once

#include <osg/Vec3>
#include <osg/Matrix>

#include <osg/StateAttribute>
#include <osgText/Text>

#define C_PI 3.14159265359f

namespace osgGaming
{
	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper)
	{
		return std::max(lower, std::min(n, upper));
	}

	void rotateVector(osg::Vec3* vec, osg::Quat quat);
	void transformVector(osg::Vec3* vec, osg::Matrixd* mat);

	osg::Quat getQuatFromEuler(double pitch, double roll, double yaw);
	osg::Matrix getMatrixFromEuler(double pitch, double roll, double yaw);
	osg::Vec3 getVec3FromEuler(double pitch, double roll, double yaw, osg::Vec3 origin = osg::Vec3(0.0, 1.0, 0.0));

	bool pointInRect(osg::Vec2f point, osg::Vec2f leftbottom, osg::Vec2f righttop);

	bool sphereLineIntersection(osg::Vec3f sphereCenter, float sphereRadius, osg::Vec3f lineOrigin, osg::Vec3f lineDirectionNornalized, osg::Vec3f& result);

	void generateTangentAndBinormal(osg::Node* node);

	osg::StateAttribute::GLModeValue glModeValueFromBool(bool on);
	std::string lowerString(std::string str);

	osg::ref_ptr<osgText::Text> createTextNode(std::string text, float characterSize, osg::ref_ptr<osgText::Font> font = NULL);
}