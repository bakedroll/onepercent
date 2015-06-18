#pragma once

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Geometry>

#include <osg/StateAttribute>
#include <osgText/Text>

#define C_PI 3.14159265359f
#define C_2PI 6.28318530718f

namespace osgGaming
{
	typedef enum _quadOrientation
	{
		XY,
		XZ,
		YZ
	} QuadOrientation;

	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper)
	{
		return std::max(lower, std::min(n, upper));
	}

	void rotateVector(osg::Vec3* vec, osg::Quat quat);
	void transformVector(osg::Vec3* vec, osg::Matrixd* mat);

	osg::Quat getQuatFromEuler(double pitch, double roll, double yaw);
	osg::Matrix getMatrixFromEuler(double pitch, double roll, double yaw);
	osg::Vec3f getVec3FromEuler(double pitch, double roll, double yaw, osg::Vec3 origin = osg::Vec3(0.0, 1.0, 0.0));

	osg::Vec2f getPolarFromCartesian(osg::Vec3f cartesian);

	osg::Vec2f getTextSize(osg::ref_ptr<osgText::Text> text);

	bool pointInRect(osg::Vec2f point, osg::Vec2f leftbottom, osg::Vec2f righttop);

	bool sphereLineIntersection(osg::Vec3f sphereCenter, float sphereRadius, osg::Vec3f lineOrigin, osg::Vec3f lineDirectionNornalized, osg::Vec3f& result);
	float pointLineDistance(osg::Vec3f origin, osg::Vec3 direction, osg::Vec3f point);

	void generateTangentAndBinormal(osg::Node* node);

	osg::StateAttribute::GLModeValue glModeValueFromBool(bool on);
	std::string lowerString(std::string str);

	osg::ref_ptr<osgText::Text> createTextNode(std::string text, float characterSize, osg::ref_ptr<osgText::Font> font = NULL);
	osg::ref_ptr<osg::Geometry> createQuadGeometry(float left, float right, float bottom, float top, float z = 0.0f, QuadOrientation orientation = XZ, bool flipped = false);
}