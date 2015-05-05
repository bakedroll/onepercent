#include <osgGaming/CameraAlignedQuad.h>

#include <osg/BlendFunc>
#include <osg/Geometry>

using namespace osgGaming;
using namespace osg;

CameraAlignedQuad::CameraAlignedQuad(int renderBin)
	: Geode()
{
	makeQuad(renderBin);
}

ref_ptr<Geometry> CameraAlignedQuad::getGeometry()
{
	return _geometry;
}

ref_ptr<Vec3Array> CameraAlignedQuad::getVertexArray()
{
	return _vertexArray;
}

ref_ptr<Vec3Array> CameraAlignedQuad::getNormalArray()
{
	return _normalArray;
}

void CameraAlignedQuad::makeQuad(int renderBin)
{
	// scattering stateset
	ref_ptr<StateSet> stateSet = new StateSet();

	stateSet->setMode(GL_BLEND, StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, StateAttribute::OFF);
	stateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
	stateSet->setAttributeAndModes(new BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), StateAttribute::ON);

	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateSet->setRenderBinDetails(renderBin, "RenderBin");

	_geometry = new Geometry();
	_geometry->setUseVertexBufferObjects(true);

	_vertexArray = new Vec3Array(4);
	_vertexArray->setDataVariance(DYNAMIC);
	_geometry->setVertexArray(_vertexArray);

	_normalArray = new Vec3Array(4);
	_normalArray->setDataVariance(DYNAMIC);
	_geometry->setNormalArray(_normalArray);
	_geometry->setNormalBinding(Geometry::BIND_PER_VERTEX);

	ref_ptr<Vec4Array> colors = new Vec4Array();
	colors->push_back(Vec4(-1.0f, -1.0f, 0.0f, 1.0f));
	colors->push_back(Vec4(-1.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(Vec4(1.0f, -1.0f, 0.0f, 1.0f));
	_geometry->setColorArray(colors);
	_geometry->setColorBinding(Geometry::BIND_PER_VERTEX);

	ref_ptr<DrawElementsUInt> indices = new DrawElementsUInt(PrimitiveSet::POLYGON, 0);
	indices->push_back(3);
	indices->push_back(2);
	indices->push_back(1);
	indices->push_back(0);
	_geometry->addPrimitiveSet(indices);

	addDrawable(_geometry);
	setStateSet(stateSet);
}