#include <osgGaming/Helper.h>

#include <algorithm>
#include <math.h>

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

Vec3f osgGaming::getVec3FromEuler(double pitch, double roll, double yaw, Vec3 origin)
{
	rotateVector(&origin, osgGaming::getQuatFromEuler(pitch, roll, yaw));

	return origin;
}

Vec2f osgGaming::getPolarFromCartesian(Vec3f cartesian)
{
	Vec2f result;

	result.x() = (atan2(-cartesian.x(), cartesian.y()) + C_PI) / (2.0f * C_PI);
	float xyLen = sqrt(cartesian.x() * cartesian.x() + cartesian.y() * cartesian.y());
	result.y() = (atan2(-cartesian.z(), xyLen) + C_PI / 2.0f) / C_PI;

	return result;
}

Vec2f osgGaming::getTextSize(ref_ptr<Text> text)
{
	BoundingBox bb;

	Text::TextureGlyphQuadMap glyphs = text->getTextureGlyphQuadMap();
	for (Text::TextureGlyphQuadMap::iterator it = glyphs.begin(); it != glyphs.end(); ++it)
	{
		Text::GlyphQuads::Coords2 quads = it->second.getCoords();
		for (Text::GlyphQuads::Coords2::iterator qit = quads.begin(); qit != quads.end(); ++qit)
		{
			bb.expandBy(qit->x(), qit->y(), 0.0f);
		}
	}

	float width = bb.xMax() - bb.xMin();
	float height = bb.yMax() - bb.yMin();

	return Vec2f(width, height);
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
float osgGaming::pointLineDistance(osg::Vec3f origin, Vec3 direction, Vec3f point)
{
	Vec3f vec = direction ^ (point - origin);

	return vec.length();
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

ref_ptr<Geometry> osgGaming::createQuadGeometry(float left, float right, float bottom, float top, float z, QuadOrientation orientation, bool flipped)
{
	ref_ptr<Geometry> geo = new Geometry();

	ref_ptr<Vec3Array> verts = new Vec3Array();
	ref_ptr<Vec3Array> normals = new Vec3Array();

	float normal = flipped ? 1.0f : -1.0f;

	switch (orientation)
	{
	case XY:
		verts->push_back(Vec3(left, bottom, z));
		verts->push_back(Vec3(left, top, z));
		verts->push_back(Vec3(right, top, z));
		verts->push_back(Vec3(right, bottom, z));

		normals->push_back(Vec3(0.0f, 0.0f, normal));
		break;
	case XZ:
		verts->push_back(Vec3(left, z, bottom));
		verts->push_back(Vec3(right, z, bottom));
		verts->push_back(Vec3(right, z, top));
		verts->push_back(Vec3(left, z, top));

		normals->push_back(Vec3(0.0f, normal, 0.0f));
		break;
	case YZ:
		verts->push_back(Vec3(z, left, bottom));
		verts->push_back(Vec3(z, left, top));
		verts->push_back(Vec3(z, right, top));
		verts->push_back(Vec3(z, right, bottom));

		normals->push_back(Vec3(normal, 0.0f, 0.0f));
		break;
	}

	ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(PrimitiveSet::POLYGON, 0);

	if (flipped)
	{
		indices->push_back(3);
		indices->push_back(2);
		indices->push_back(1);
		indices->push_back(0);
	}
	else
	{
		indices->push_back(0);
		indices->push_back(1);
		indices->push_back(2);
		indices->push_back(3);
	}

	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	ref_ptr<Vec2Array> texcoords = new Vec2Array();
	texcoords->push_back(Vec2(0.0f, 0.0f));
	texcoords->push_back(Vec2(1.0f, 0.0f));
	texcoords->push_back(Vec2(1.0f, 1.0f));
	texcoords->push_back(Vec2(0.0f, 1.0f));

	geo->setTexCoordArray(0, texcoords);
	geo->addPrimitiveSet(indices);
	geo->setVertexArray(verts);
	geo->setNormalArray(normals);
	geo->setNormalBinding(Geometry::BIND_OVERALL);
	geo->setColorArray(colors);
	geo->setColorBinding(osg::Geometry::BIND_OVERALL);

	return geo;
}