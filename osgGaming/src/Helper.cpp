#include <osgGaming/Helper.h>

#include <algorithm>

#include <osg/Geode>
#include <osgUtil/TangentSpaceGenerator>

using namespace osg;
using namespace osgText;
using namespace std;

// Z  Y
// | /
// |/
// +----- X

void osgGaming::rotateVector(Vec3* vec, Quat quat)
{
	Matrixd mat = Matrixd::identity();
	mat.setRotate(quat);

	transformVector(vec, &mat);
}

void osgGaming::transformVector(Vec3* vec, Matrixd* mat)
{
	Vec3 cpy(*vec);

	(*vec)[0] = (*mat)(0, 0)*cpy[0] + (*mat)(1, 0)*cpy[1] + (*mat)(2, 0)*cpy[2] + (*mat)(3, 0);
	(*vec)[1] = (*mat)(0, 1)*cpy[0] + (*mat)(1, 1)*cpy[1] + (*mat)(2, 1)*cpy[2] + (*mat)(3, 1);
	(*vec)[2] = (*mat)(0, 2)*cpy[0] + (*mat)(1, 2)*cpy[1] + (*mat)(2, 2)*cpy[2] + (*mat)(3, 2);
}

Quat osgGaming::getQuatFromEuler(double pitch, double roll, double yaw)
{
	Quat q;

	double c1 = cos(roll / 2.0);
	double s1 = sin(roll / 2.0);
	double c2 = cos(yaw / 2.0);
	double s2 = sin(yaw / 2.0);
	double c3 = cos(pitch / 2.0);
	double s3 = sin(pitch / 2.0);

	double c1c2 = c1*c2;
	double s1s2 = s1*s2;

	q[0] = c1c2*s3 + s1s2*c3;
	q[1] = s1*c2*c3 + c1*s2*s3;
	q[2] = c1*s2*c3 - s1*c2*s3;
	q[3] = c1c2*c3 - s1s2*s3;

	return q;
}

Matrix osgGaming::getMatrixFromEuler(double pitch, double roll, double yaw)
{
	Quat quat = getQuatFromEuler(pitch, roll, yaw);

	Matrix mat;
	mat.setRotate(quat);

	return mat;
}

Vec3 osgGaming::getVec3FromEuler(double pitch, double roll, double yaw, Vec3 origin)
{
	rotateVector(&origin, osgGaming::getQuatFromEuler(pitch, roll, yaw));

	return origin;
}

bool osgGaming::pointInRect(Vec2f point, Vec2f leftbottom, Vec2f righttop)
{
	return (point.x() >= leftbottom.x() && point.y() >= leftbottom.y()
		&& point.x() <= righttop.x() && point.y() <= righttop.y());
}

bool osgGaming::sphereLineIntersection(Vec3f sphereCenter, float sphereRadius, Vec3f lineOrigin, Vec3f lineDirectionNornalized, Vec3f& result)
{
	float a = lineDirectionNornalized * lineDirectionNornalized;
	float b = lineDirectionNornalized * ((lineOrigin - sphereCenter) * 2.0f);
	float c = (sphereCenter * sphereCenter) + (lineOrigin * lineOrigin) - 2.0f * (lineOrigin * sphereCenter) - sphereRadius * sphereRadius;
	float D = b * b + (-4.0f) * a * c;

	if (D < 0)
	{
		return false;
	}

	D = sqrtf(D);

	float t = (-0.5f) * (b + D) / a;
	if (t > 0.0f)
	{
		result = lineOrigin + lineDirectionNornalized * t;
	}
	else
	{
		return false;
	}

	return true;
}

void osgGaming::generateTangentAndBinormal(Node* node)
{
	ref_ptr<Group> group = node->asGroup();
	ref_ptr<Geode> geode = node->asGeode();

	if (group)
	{
		for (unsigned int i = 0; i<group->getNumChildren(); i++)
			generateTangentAndBinormal(group->getChild(i));
	}
	else if (geode)
	{
		for (unsigned int i = 0; i<geode->getNumDrawables(); i++)
		{
			Geometry *geometry = geode->getDrawable(i)->asGeometry();
			if (geometry)
			{
				ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator();
				tsg->generate(geometry);

				geometry->setVertexAttribArray(6, tsg->getTangentArray());
				geometry->setVertexAttribBinding(6, osg::Geometry::BIND_PER_VERTEX);

				geometry->setVertexAttribArray(7, tsg->getBinormalArray());
				geometry->setVertexAttribBinding(7, osg::Geometry::BIND_PER_VERTEX);
				geometry->setVertexAttribNormalize(7, GL_FALSE);

				geometry->setUseVertexBufferObjects(true);
				//geometry->getVertexAttribArray(6)->dirty();
				//geometry->getVertexAttribArray(7)->dirty();

				tsg.release();
			}
		}
	}
}

StateAttribute::GLModeValue osgGaming::glModeValueFromBool(bool on)
{
	return on ? StateAttribute::ON : StateAttribute::OFF;
}

string osgGaming::lowerString(string str)
{
	transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

ref_ptr<Text> osgGaming::createTextNode(string text, float characterSize, ref_ptr<Font> font)
{
	ref_ptr<Text> textNode = new Text();

	if (font.valid())
	{
		textNode->setFont(font);
	}

	textNode->setCharacterSize(characterSize);
	textNode->setText(text);
	textNode->setAxisAlignment(osgText::Text::SCREEN);
	textNode->setDrawMode(osgText::Text::TEXT);
	textNode->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	textNode->setDataVariance(osg::Object::DYNAMIC);

	return textNode;
}