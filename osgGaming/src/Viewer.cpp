#include <osgGaming/Viewer.h>

#include <osgPPU/UnitOut.h>

using namespace osgGaming;
using namespace osgPPU;
using namespace osg;

Viewer::Viewer()
	: osgViewer::Viewer(),
	  _resolutionKnown(false),
	  _ppuInitialized(false)
{
	_ppGroup = new Group();
	osgViewer::Viewer::setSceneData(_ppGroup);
}

void Viewer::updateResolution(float width, float height)
{
	_resolution = Vec2f(width, height);

	_resolutionKnown = true;
}

void Viewer::setSceneData(Node* node)
{
	if (_ppSceneData.valid())
	{
		_ppGroup->removeChild(_ppSceneData);
	}

	_ppSceneData = node;

	if (_ppSceneData.valid())
	{
		_ppGroup->addChild(_ppSceneData);
	}
}

ref_ptr<Group> Viewer::getRootGroup()
{
	return _ppGroup;
}

ref_ptr<Unit> Viewer::bypassUnit(Camera::BufferComponent bufferComponent)
{
	RenderTextureDictionary::iterator it = _renderTextures.find(bufferComponent);
	if (it != _renderTextures.end())
	{
		return it->second.bypassUnit;
	}

	ref_ptr<Texture2D> texture = new Texture2D();
	ref_ptr<Unit> bypassUnit;

	if (_resolutionKnown)
	{
		texture->setTextureSize((int)_resolution.x(), (int)_resolution.y());
	}

	texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
	texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);
	texture->setBorderColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	switch (bufferComponent)
	{
	case Camera::COLOR_BUFFER:
		texture->setInternalFormat(GL_RGBA16F_ARB);
		texture->setSourceFormat(GL_RGBA);
		texture->setSourceType(GL_FLOAT);

		break;

	case Camera::DEPTH_BUFFER:
		texture->setInternalFormat(GL_DEPTH_COMPONENT);

		break;
	}


	getCamera()->attach(bufferComponent, texture);

	return bypassUnit;
}

void Viewer::initializePPU()
{
	if (_ppuInitialized)
	{
		return;
	}

	ref_ptr<Camera> camera = getCamera();

	_processor = new Processor();
	_processor->setCamera(camera);

	camera->setComputeNearFarMode(CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setRenderTargetImplementation(Camera::FRAME_BUFFER_OBJECT);

	_outputUnit = new UnitOut();
	_outputUnit->setInputTextureIndexForViewportReference(-1);

	_ppGroup->addChild(_processor);
	_processor->addChild(_outputUnit);

	_ppuInitialized = true;
}