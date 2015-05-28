#include <osgGaming/Viewer.h>

#include <osgPPU/UnitOut.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitDepthbufferBypass.h>
#include <osgPPU/Camera.h>

using namespace osgGaming;
using namespace osgPPU;
using namespace osg;

Viewer::Viewer()
	: osgViewer::Viewer(),
	  _resolutionKnown(false),
	  _ppuInitialized(false)
{
	initialize();
}

void Viewer::updateResolution(float width, float height)
{
	_resolution = Vec2f(width, height);

	if (_processor.valid())
	{
		osgPPU::Camera::resizeViewport(0, 0, width, height, getCamera());
		_processor->onViewportChange();
	}

	for (RenderTextureDictionary::iterator it = _renderTextures.begin(); it != _renderTextures.end(); ++it)
	{
		it->second.texture->setTextureSize(width, height);
	}

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

void Viewer::setClampColorEnabled(bool enabled)
{
	if (!_clampColor.valid())
	{
		_clampColor = new ClampColor();
		_clampColor->setClampVertexColor(GL_FALSE);
		_clampColor->setClampFragmentColor(GL_FALSE);
		_clampColor->setClampReadColor(GL_FALSE);
	}

	_ppGroup->getOrCreateStateSet()->setAttribute(_clampColor, enabled ? StateAttribute::ON : StateAttribute::OFF);
}

ref_ptr<Group> Viewer::getRootGroup()
{
	return _ppGroup;
}

void Viewer::addPostProcessingEffect(ref_ptr<PostProcessingEffect> ppe)
{
	initializePPU();

	ppe->initialize();

	ref_ptr<osgPPU::Unit> unitOut = new UnitOut();
	unitOut->setInputTextureIndexForViewportReference(-1);

	//PostProcessingEffect::SwitchList switchList;

	PostProcessingEffect::InitialUnitList initialUnits = ppe->getInitialUnits();
	for (PostProcessingEffect::InitialUnitList::iterator it = initialUnits.begin(); it != initialUnits.end(); ++it)
	{
		//ref_ptr<Switch> s = new Switch();

		unitForType(it->type)->addChild(it->unit);
		//unitForType(it->type)->addChild(s);
		//s->addChild(it->unit, true);

		//if (it->type == PostProcessingEffect::ONGOING_COLOR)
		//{
		//	s->addChild(unitOut, false);
		//}

		//switchList.push_back(s);
	}

	PostProcessingEffect::InputToUniformList inputToUniformList = ppe->getInputToUniform();
	for (PostProcessingEffect::InputToUniformList::iterator it = inputToUniformList.begin(); it != inputToUniformList.end(); ++it)
	{
		it->unit->setInputToUniform(unitForType(it->type), it->name, true);
	}

	_lastUnit = ppe->getResultUnit();


	_lastUnit->addChild(unitOut);

	// _processor->dirtyUnitSubgraph();

	_ppeList.push_back(ppe);
}

void Viewer::initialize()
{
	_ppGroup = new Group();

	osgViewer::Viewer::setSceneData(_ppGroup);

	//osg::ref_ptr<osg::Camera> camera = getCamera();

	//camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT, osg::Camera::FRAME_BUFFER);
}

ref_ptr<Unit> Viewer::bypassUnit(osg::Camera::BufferComponent bufferComponent)
{
	RenderTextureDictionary::iterator it = _renderTextures.find(bufferComponent);
	if (it != _renderTextures.end())
	{
		return it->second.bypassUnit;
	}

	RenderTexture rt;

	rt.texture = new Texture2D();

	if (_resolutionKnown)
	{
		rt.texture->setTextureSize((int)_resolution.x(), (int)_resolution.y());
	}

	rt.texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	rt.texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	rt.texture->setResizeNonPowerOfTwoHint(false);
	rt.texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
	rt.texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);
	rt.texture->setBorderColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	switch (bufferComponent)
	{
	case osg::Camera::COLOR_BUFFER:
		rt.texture->setInternalFormat(GL_RGBA16F_ARB);
		rt.texture->setSourceFormat(GL_RGBA);
		rt.texture->setSourceType(GL_FLOAT);

		rt.bypassUnit = new osgPPU::UnitBypass();
		_processor->addChild(rt.bypassUnit);

		break;

	case osg::Camera::DEPTH_BUFFER:
		rt.texture->setInternalFormat(GL_DEPTH_COMPONENT);

		rt.bypassUnit = new osgPPU::UnitDepthbufferBypass();
		_processor->addChild(rt.bypassUnit);

		break;
	}

	getCamera()->attach(bufferComponent, rt.texture);

	_renderTextures.insert(RenderTextureDictionary::value_type(bufferComponent, rt));

	return rt.bypassUnit;
}

ref_ptr<osgPPU::Unit> Viewer::lastUnit()
{
	if (!_lastUnit.valid())
	{
		_lastUnit = bypassUnit(osg::Camera::COLOR_BUFFER);
	}

	return _lastUnit;
}

ref_ptr<osgPPU::Unit> Viewer::unitForType(PostProcessingEffect::UnitType type)
{
	ref_ptr<osgPPU::Unit> unit;

	switch (type)
	{
	case PostProcessingEffect::BYPASS_COLOR:

		unit = bypassUnit(osg::Camera::COLOR_BUFFER);
		break;

	case PostProcessingEffect::BYPASS_DEPTH:

		unit = bypassUnit(osg::Camera::DEPTH_BUFFER);
		break;

	case PostProcessingEffect::ONGOING_COLOR:

		unit = lastUnit();
		break;

	}

	return unit;
}

void Viewer::initializePPU()
{
	if (_ppuInitialized)
	{
		return;
	}

	ref_ptr<osg::Camera> camera = getCamera();

	_processor = new Processor();
	_processor->setCamera(camera);

	camera->setComputeNearFarMode(CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_ppGroup->addChild(_processor);


	if (_resolutionKnown)
	{
		osgPPU::Camera::resizeViewport(0, 0, _resolution.x(), _resolution.y(), getCamera());
		_processor->onViewportChange();
	}

	_processor->dirtyUnitSubgraph();

	_ppuInitialized = true;
}