#include <osgGaming/Viewer.h>

#include <osg/Projection>
#include <osgViewer/Renderer>

#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitDepthbufferBypass.h>
#include <osgPPU/Camera.h>

using namespace osgGaming;
using namespace osgPPU;
using namespace osg;
using namespace std;


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

	_resolutionKnown = true;

	ref_ptr<osg::Camera> camera = getCamera();
	ref_ptr<Viewport> vp = new Viewport(0, 0, width, height);

	if (_processor.valid())
	{
		osgPPU::Camera::resizeViewport(0, 0, width, height, camera);
		_processor->onViewportChange();
	}

	camera->setViewport(vp);
	// _hudCamera->setViewport(vp);

	updateCameraRenderTextures(true);
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

ref_ptr<Hud> Viewer::getHud()
{
	return _hud;
}

void Viewer::setHud(ref_ptr<Hud> hud)
{
	if (_hud.valid())
	{
		_hudSwitch->setChildValue(_hud->getProjection(), false);
	}

	_hud = hud;

	if (!_hudSwitch->containsNode(_hud->getProjection()))
	{
		_hudSwitch->addChild(_hud->getProjection() , true);
	}
	else
	{
		_hudSwitch->setChildValue(_hud->getProjection(), true);
	}
}

void Viewer::addPostProcessingEffect(ref_ptr<PostProcessingEffect> ppe, bool enabled, string name)
{
	if (enabled)
	{
		resetPostProcessingEffects();
	}

	PostProcessingState pps;
	pps.effect = ppe;
	pps.enabled = enabled;

	if (name.empty())
	{
		name = ppe->getName();
	}

	_ppeDictionary.insert(PostProcessingStateDictionary::value_type(name, pps));

	if (enabled)
	{
		setupPostProcessingEffects();
	}
}

void Viewer::setPostProcessingEffectEnabled(string ppeName, bool enabled)
{
	PostProcessingStateDictionary::iterator it = _ppeDictionary.find(ppeName);

	if (it->second.enabled == enabled)
	{
		return;
	}

	printf("Post processing effect '%s': %s\n", ppeName.c_str(), enabled ? "enabled" : "disabled");

	resetPostProcessingEffects();

	it->second.enabled = enabled;

	setupPostProcessingEffects();
}

void Viewer::setPostProcessingEffectEnabled(unsigned int index, bool enabled)
{
	setPostProcessingEffectEnabled(postProcessingEffectName(index), enabled);
}

ref_ptr<PostProcessingEffect> Viewer::getPostProcessingEffect(string ppeName)
{
	return _ppeDictionary.find(ppeName)->second.effect;
}

ref_ptr<PostProcessingEffect> Viewer::getPostProcessingEffect(unsigned int index)
{
	return getPostProcessingEffect(postProcessingEffectName(index));
}

bool Viewer::getPostProcessingEffectEnabled(string ppeName)
{
	PostProcessingStateDictionary::iterator it = _ppeDictionary.find(ppeName);

	return it->second.enabled;
}

bool Viewer::getPostProcessingEffectEnabled(unsigned int index)
{
	return getPostProcessingEffectEnabled(postProcessingEffectName(index));
}

bool Viewer::hasPostProcessingEffect(string ppeName)
{
	return _ppeDictionary.find(ppeName) != _ppeDictionary.end();
}

void Viewer::initialize()
{
	setThreadingModel(ViewerBase::SingleThreaded);

	_ppGroup = new Group();

	osgViewer::Viewer::setSceneData(_ppGroup);

	ref_ptr<osg::Camera> camera = getCamera();
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT, osg::Camera::FRAME_BUFFER);
	camera->setComputeNearFarMode(CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	//camera->attach(osg::Camera::COLOR_BUFFER, texture, 0, 0, false, 8, 8);

	_hudCamera = new osg::Camera();
	_hudCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER, osg::Camera::FRAME_BUFFER);
	_hudCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	_hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	_hudCamera->setViewMatrix(Matrixd::identity());
	_hudCamera->setProjectionMatrix(Matrixd::identity());

	// implicitly attaches texture to camera
	ref_ptr<Texture2D> texture = renderTexture(osg::Camera::COLOR_BUFFER).texture;

	ref_ptr<Geode> geode = new Geode();
	geode->addDrawable(createTexturedQuadGeometry(Vec3f(-1.0f, -1.0f, 0.0f), Vec3f(2.0f, 0.0f, 0.0f), Vec3f(0.0f, 2.0f, 0.0f)));

	_hudStateSet = geode->getOrCreateStateSet();
	_hudStateSet->setTextureAttributeAndModes(0, texture, StateAttribute::ON);
	_hudStateSet->setMode(GL_LIGHTING, StateAttribute::OFF);

	_hudCamera->addChild(geode);

	_hudSwitch = new Switch();
	_hudCamera->addChild(_hudSwitch);

	_ppGroup->addChild(_hudCamera);
}

void Viewer::resetPostProcessingEffects()
{
	initializePPU();

	_lastUnit = lastUnit(true);

	for (PostProcessingStateDictionary::iterator it = _ppeDictionary.begin(); it != _ppeDictionary.end(); ++it)
	{
		if (!it->second.enabled)
		{
			continue;
		}

		ref_ptr<PostProcessingEffect> ppe = it->second.effect;

		PostProcessingEffect::InitialUnitList initialUnits = ppe->getInitialUnits();
		for (PostProcessingEffect::InitialUnitList::iterator it = initialUnits.begin(); it != initialUnits.end(); ++it)
		{
			unitForType(it->type)->removeChild(it->unit);
		}

		PostProcessingEffect::InputToUniformList inputToUniformList = ppe->getInputToUniform();
		for (PostProcessingEffect::InputToUniformList::iterator it = inputToUniformList.begin(); it != inputToUniformList.end(); ++it)
		{
			unitForType(it->type)->removeChild(it->unit);

			//it->unit->setInputToUniform(unitForType(it->type), it->name, false);
		}

		_lastUnit = ppe->getResultUnit();
	}

	if (_unitOutput.valid())
	{
		_lastUnit->removeChild(_unitOutput);
	}

	_unitOutput = new UnitInOut();
	_unitOutput->setInputTextureIndexForViewportReference(-1);
	_hudStateSet->setTextureAttributeAndModes(0, _unitOutput->getOrCreateOutputTexture(0), StateAttribute::ON);
}

void Viewer::setupPostProcessingEffects()
{
	_lastUnit = lastUnit(true);

	for (PostProcessingStateDictionary::iterator it = _ppeDictionary.begin(); it != _ppeDictionary.end(); ++it)
	{
		if (!it->second.enabled)
		{
			continue;
		}

		ref_ptr<PostProcessingEffect> ppe = it->second.effect;

		ppe->initialize();

		PostProcessingEffect::InitialUnitList initialUnits = ppe->getInitialUnits();
		for (PostProcessingEffect::InitialUnitList::iterator it = initialUnits.begin(); it != initialUnits.end(); ++it)
		{
			unitForType(it->type)->addChild(it->unit);
		}

		PostProcessingEffect::InputToUniformList inputToUniformList = ppe->getInputToUniform();
		for (PostProcessingEffect::InputToUniformList::iterator it = inputToUniformList.begin(); it != inputToUniformList.end(); ++it)
		{
			it->unit->setInputToUniform(unitForType(it->type), it->name, true);
		}

		_lastUnit = ppe->getResultUnit();
	}

	_lastUnit->addChild(_unitOutput);

	updateCameraRenderTextures();
	_processor->dirtyUnitSubgraph();
}

void Viewer::updateCameraRenderTextures(bool recreate)
{
	for (RenderTextureDictionary::iterator it = _renderTextures.begin(); it != _renderTextures.end(); ++it)
	{
		ref_ptr<Texture2D> tex = renderTexture((osg::Camera::BufferComponent)it->first, recreate).texture;
		if (it->first == osg::Camera::COLOR_BUFFER && !_processor.valid())
		{
			_hudStateSet->setTextureAttributeAndModes(0, tex, StateAttribute::ON);
		}
	}

	osgViewer::Renderer* renderer = (osgViewer::Renderer*)getCamera()->getRenderer();
	renderer->getSceneView(0)->getRenderStage()->setCameraRequiresSetUp(true);
	renderer->getSceneView(0)->getRenderStage()->setFrameBufferObject(NULL);
}

Viewer::RenderTexture Viewer::renderTexture(osg::Camera::BufferComponent bufferComponent, bool recreate)
{
	RenderTextureDictionary::iterator it = _renderTextures.find(bufferComponent);
	if (it != _renderTextures.end())
	{
		if (recreate)
		{
			RenderTexture rt = it->second;

			getCamera()->detach(bufferComponent);
			rt.texture = createRenderTexture(bufferComponent);

			_renderTextures[bufferComponent] = rt;

			return rt;
		}

		return it->second;
	}

	RenderTexture rt;

	rt.texture = createRenderTexture(bufferComponent);

	_renderTextures.insert(RenderTextureDictionary::value_type(bufferComponent, rt));

	return rt;
}

ref_ptr<Texture2D> Viewer::createRenderTexture(osg::Camera::BufferComponent bufferComponent)
{
	ref_ptr<Texture2D> texture = new Texture2D();

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
	case osg::Camera::COLOR_BUFFER:
		texture->setInternalFormat(GL_RGBA16F_ARB);
		texture->setSourceFormat(GL_RGBA);
		texture->setSourceType(GL_FLOAT);

		break;

	case osg::Camera::DEPTH_BUFFER:
		texture->setInternalFormat(GL_DEPTH_COMPONENT);

		break;
	}

	getCamera()->attach(bufferComponent, texture);

	return texture;
}

ref_ptr<Unit> Viewer::bypassUnit(osg::Camera::BufferComponent bufferComponent)
{
	RenderTexture rt = renderTexture(bufferComponent);

	if (!rt.bypassUnit.valid())
	{
		switch (bufferComponent)
		{
		case osg::Camera::COLOR_BUFFER:
			rt.bypassUnit = new osgPPU::UnitBypass();
			_processor->addChild(rt.bypassUnit);

			break;

		case osg::Camera::DEPTH_BUFFER:
			rt.bypassUnit = new osgPPU::UnitDepthbufferBypass();
			_processor->addChild(rt.bypassUnit);

			break;
		}

		_renderTextures[bufferComponent] = rt;
	}

	return rt.bypassUnit;
}

ref_ptr<osgPPU::Unit> Viewer::lastUnit(bool reset)
{
	if (!_lastUnit.valid() || reset)
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

string Viewer::postProcessingEffectName(unsigned int index)
{
	unsigned int c = 0;

	for (PostProcessingStateDictionary::iterator it = _ppeDictionary.begin(); it != _ppeDictionary.end(); ++it)
	{
		if (c == index)
		{
			return it->first;
		}

		c++;
	}

	return "";
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

	_ppGroup->addChild(_processor);

	if (_resolutionKnown)
	{
		osgPPU::Camera::resizeViewport(0, 0, _resolution.x(), _resolution.y(), getCamera());
		_processor->onViewportChange();
	}

	_ppuInitialized = true;
}