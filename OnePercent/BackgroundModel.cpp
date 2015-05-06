#include "BackgroundModel.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Point>

#include <osgGaming/ResourceManager.h>

#include <fstream>

using namespace onep;
using namespace osgGaming;
using namespace osg;
using namespace std;


BackgroundModel::BackgroundModel(string starsFilename)
	: PositionAttitudeTransform()
{
	makeStars(starsFilename);
}

void BackgroundModel::makeStars(string starsFilename)
{
	char* bytes = ResourceManager::getInstance()->loadBinary(starsFilename);

	ref_ptr<Geode> geode = new Geode();
	ref_ptr<StateSet> stateSet = new StateSet();

	int nstars;
	memcpy(&nstars, bytes, sizeof(int));

	int position = sizeof(int);

	for (int i = 0; i < nstars; i++)
	{
		float size, x, y, z;

		memcpy(&x, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&y, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&z, &bytes[position], sizeof(float));
		position += sizeof(float);

		memcpy(&size, &bytes[position], sizeof(float));
		position += sizeof(float);

		ref_ptr<Geometry> geo = new Geometry();

		ref_ptr<Vec3Array> verts = new Vec3Array();
		verts->push_back(Vec3f(x, y, z) * 10.0f);
		geo->setVertexArray(verts);

		Vec3f color = Vec3f(1.0f, 1.0f, 1.0f) * size * 0.05f;

		ref_ptr<Vec4Array> colors = new Vec4Array();
		colors->push_back(Vec4f(color.x(), color.y(), color.z(), 1.0f));
		geo->setColorArray(colors);
		geo->setColorBinding(Geometry::BIND_OVERALL);

		geo->addPrimitiveSet(new DrawArrays(PrimitiveSet::POINTS, 0, verts->size()));

		geode->addDrawable(geo);
	}

	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(-10, "RenderBin");

	geode->setStateSet(stateSet);
	addChild(geode);

	ResourceManager::getInstance()->clearCacheResource(starsFilename);
}