#include <osgGaming/View.h>

#include <osg/Texture2D>
#include <osg/ClampColor>
#include <osg/Switch>
#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>
#include <osgPPU/UnitInOut.h>

#include <osgViewer/ViewerBase>
#include <osgViewer/Renderer>

#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitDepthbufferBypass.h>
#include <osgPPU/Camera.h>

#include <assert.h>
#include <osgGaming/ResourceManager.h>

#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>

using namespace osgPPU;
using namespace osg;
using namespace std;

namespace osgGaming
{

struct View::Impl
{
  Impl()
    : fullscreenEnabled(true)
    , resolutionInitialized(false)
    , screenNum(0)
    , ppuInitialized(false)
  {
  }

  typedef struct _renderTexture
  {
    osg::ref_ptr<osg::Texture2D> texture;
    osg::ref_ptr<osgPPU::Unit> bypassUnit;
  } RenderTexture;

  typedef struct _postProcessingState
  {
    osg::ref_ptr<PostProcessingEffect> effect;
    bool enabled;
  } PostProcessingState;

  typedef std::map<int, RenderTexture> RenderTextureDictionary;
  typedef std::map<std::string, PostProcessingState> PostProcessingStateDictionary;

  osg::Vec2f resolution;
  osg::Vec4f windowRect;
  bool fullscreenEnabled;
  bool resolutionInitialized;
  int screenNum;

  osg::ref_ptr<osg::Node> ppSceneData;
  osg::ref_ptr<osg::StateSet> hudStateSet;
  osg::ref_ptr<osg::Camera> sceneCamera;
  osg::ref_ptr<osg::Camera> hudCamera;
  osg::ref_ptr<osg::Switch> hudSwitch;
  osg::ref_ptr<Hud> hud;

  osg::ref_ptr<osgPPU::Processor> processor;
  osg::ref_ptr<osg::Group> ppGroup;
  osg::ref_ptr<osg::Group> hudGroup;
  osg::ref_ptr<osg::ClampColor> clampColor;

  osg::ref_ptr<osgPPU::Unit> lastUnit;
  osg::ref_ptr<osgPPU::UnitInOut> unitOutput;

  bool ppuInitialized;
  RenderTextureDictionary renderTextures;
  PostProcessingStateDictionary ppeDictionary;

  void initialize(osg::ref_ptr<osg::Camera> camera)
  {
    //sceneCamera = new osg::Camera();
    sceneCamera = camera;
    sceneCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT, osg::Camera::FRAME_BUFFER);
    sceneCamera->setComputeNearFarMode(CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    sceneCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sceneCamera->setRenderOrder(osg::Camera::PRE_RENDER);

    //hudCamera = camera;
    hudCamera = new osg::Camera();
    hudCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER, osg::Camera::FRAME_BUFFER);
    hudCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
    hudCamera->setViewMatrix(Matrixd::identity());
    hudCamera->setProjectionMatrix(Matrixd::identity());

    ppGroup = new Group();

    // implicitly attaches texture to camera
    ref_ptr<Texture2D> texture = renderTexture(osg::Camera::COLOR_BUFFER).texture;

    ref_ptr<Geode> geode = new Geode();
    geode->addDrawable(createTexturedQuadGeometry(Vec3f(-1.0f, -1.0f, 0.0f), Vec3f(2.0f, 0.0f, 0.0f), Vec3f(0.0f, 2.0f, 0.0f)));

    /*
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
    transform->addChild(geode);
    //transform->setPosition(Vec3d(50, 0, 0));

    osg::ref_ptr<osg::ShapeDrawable> shapedr = new osg::ShapeDrawable(new osg::Sphere());
    geode->addDrawable(shapedr);



    hudStateSet = geode->getOrCreateStateSet();
    hudStateSet->setTextureAttributeAndModes(0, texture, StateAttribute::ON);
    hudStateSet->setMode(GL_LIGHTING, StateAttribute::OFF);
    hudStateSet->setMode(GL_CULL_FACE, StateAttribute::OFF);

    hudSwitch = new Switch();

    hudGroup = new Group();
    hudGroup->addChild(transform);
    hudGroup->addChild(hudSwitch);
    hudGroup->addChild(sceneCamera);

    sceneCamera->addChild(ppGroup);
    */

    hudStateSet = geode->getOrCreateStateSet();
    hudStateSet->setTextureAttributeAndModes(0, texture, StateAttribute::ON);
    hudStateSet->setMode(GL_LIGHTING, StateAttribute::OFF);

    hudCamera->addChild(geode);

    hudSwitch = new Switch();
    hudCamera->addChild(hudSwitch);

    ppGroup->addChild(hudCamera);
  }

  void resetPostProcessingEffects()
  {
    initializePPU();

    lastUnit = getLastUnit(true);

    for (PostProcessingStateDictionary::iterator it = ppeDictionary.begin(); it != ppeDictionary.end(); ++it)
    {
      if (!it->second.enabled)
      {
        continue;
      }

      ref_ptr<PostProcessingEffect> ppe = it->second.effect;

      PostProcessingEffect::InitialUnitList initialUnits = ppe->getInitialUnits();
      for (PostProcessingEffect::InitialUnitList::iterator iit = initialUnits.begin(); iit != initialUnits.end(); ++iit)
      {
        unitForType(iit->type)->removeChild(iit->unit);
      }

      PostProcessingEffect::InputToUniformList inputToUniformList = ppe->getInputToUniform();
      for (PostProcessingEffect::InputToUniformList::iterator iit = inputToUniformList.begin(); iit != inputToUniformList.end(); ++iit)
      {
        unitForType(iit->type)->removeChild(iit->unit);
      }

      lastUnit = ppe->getResultUnit();
    }

    if (unitOutput.valid())
    {
      lastUnit->removeChild(unitOutput);
    }

    unitOutput = new UnitInOut();
    unitOutput->setInputTextureIndexForViewportReference(-1);
    hudStateSet->setTextureAttributeAndModes(0, unitOutput->getOrCreateOutputTexture(0), StateAttribute::ON);
  }

  void setupPostProcessingEffects()
  {
    lastUnit = getLastUnit(true);

    for (PostProcessingStateDictionary::iterator it = ppeDictionary.begin(); it != ppeDictionary.end(); ++it)
    {
      if (!it->second.enabled)
        continue;

      ref_ptr<PostProcessingEffect> ppe = it->second.effect;

      ppe->initialize();

      PostProcessingEffect::InitialUnitList initialUnits = ppe->getInitialUnits();
      for (PostProcessingEffect::InitialUnitList::iterator uit = initialUnits.begin(); uit != initialUnits.end(); ++uit)
      {
        unitForType(uit->type)->addChild(uit->unit);
      }

      PostProcessingEffect::InputToUniformList inputToUniformList = ppe->getInputToUniform();
      for (PostProcessingEffect::InputToUniformList::iterator iit = inputToUniformList.begin(); iit != inputToUniformList.end(); ++iit)
      {
        iit->unit->setInputToUniform(unitForType(iit->type), iit->name, true);
      }

      lastUnit = ppe->getResultUnit();
    }

    lastUnit->addChild(unitOutput);

    updateCameraRenderTextures();
    processor->dirtyUnitSubgraph();
  }

  void updateWindowRect(const osgViewer::ViewerBase::Windows& windows)
  {
    if (resolutionInitialized)
    {
      ref_ptr<osgViewer::GraphicsWindow> graphicsWindow = *windows.begin();

      if (graphicsWindow.valid())
      {
        if (fullscreenEnabled)
        {
          unsigned int screenWidth, screenHeight;
          GraphicsContext::getWindowingSystemInterface()->getScreenResolution(GraphicsContext::ScreenIdentifier(screenNum), screenWidth, screenHeight);

          graphicsWindow->setWindowDecoration(false);
          graphicsWindow->setWindowRectangle(0, 0, screenWidth, screenHeight);
        }
        else
        {
          graphicsWindow->setWindowDecoration(true);
          graphicsWindow->setWindowRectangle(
            int(windowRect.x()),
            int(windowRect.y()),
            int(windowRect.z()),
            int(windowRect.w()));
        }

        // graphicsWindow->grabFocusIfPointerInWindow();
      }
    }
  }

  void updateCameraRenderTextures(bool recreate = false)
  {
    for (RenderTextureDictionary::iterator it = renderTextures.begin(); it != renderTextures.end(); ++it)
    {
      ref_ptr<Texture2D> tex = renderTexture(osg::Camera::BufferComponent(it->first), recreate).texture;
      if (it->first == osg::Camera::COLOR_BUFFER && !processor.valid())
      {
        hudStateSet->setTextureAttributeAndModes(0, tex, StateAttribute::ON);
      }
    }

    osgViewer::Renderer* renderer = static_cast<osgViewer::Renderer*>(sceneCamera->getRenderer());
    renderer->getSceneView(0)->getRenderStage()->setCameraRequiresSetUp(true);
    renderer->getSceneView(0)->getRenderStage()->setFrameBufferObject(NULL);
  }

  RenderTexture renderTexture(osg::Camera::BufferComponent bufferComponent, bool recreate = false)
  {
    RenderTextureDictionary::iterator it = renderTextures.find(bufferComponent);
    if (it != renderTextures.end())
    {
      if (recreate)
      {
        RenderTexture rt = it->second;

        sceneCamera->detach(bufferComponent);
        rt.texture = createRenderTexture(bufferComponent);

        renderTextures[bufferComponent] = rt;

        return rt;
      }

      return it->second;
    }

    RenderTexture rt;

    rt.texture = createRenderTexture(bufferComponent);

    renderTextures.insert(RenderTextureDictionary::value_type(bufferComponent, rt));

    return rt;
  }

  osg::ref_ptr<osg::Texture2D> createRenderTexture(osg::Camera::BufferComponent bufferComponent)
  {
    ref_ptr<Texture2D> texture = new Texture2D();

    texture->setTextureSize(int(resolution.x()), int(resolution.y()));
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
      //texture->setInternalFormat(GL_RGB16F_ARB);
      //texture->setSourceFormat(GL_RGB);
      //texture->setSourceType(GL_HALF_FLOAT);


      break;

    case osg::Camera::DEPTH_BUFFER:
      texture->setInternalFormat(GL_DEPTH_COMPONENT);

      break;
    }

    sceneCamera->attach(bufferComponent, texture);

    return texture;
  }

  osg::ref_ptr<osgPPU::Unit> bypassUnit(osg::Camera::BufferComponent bufferComponent)
  {
    RenderTexture rt = renderTexture(bufferComponent);

    if (!rt.bypassUnit.valid())
    {
      switch (bufferComponent)
      {
      case osg::Camera::COLOR_BUFFER:
        rt.bypassUnit = new osgPPU::UnitBypass();
        processor->addChild(rt.bypassUnit);

        break;

      case osg::Camera::DEPTH_BUFFER:
        rt.bypassUnit = new osgPPU::UnitDepthbufferBypass();
        processor->addChild(rt.bypassUnit);

        break;
      }

      renderTextures[bufferComponent] = rt;
    }

    return rt.bypassUnit;
  }

  osg::ref_ptr<osgPPU::Unit> getLastUnit(bool reset = false)
  {
    if (!lastUnit.valid() || reset)
      lastUnit = bypassUnit(osg::Camera::COLOR_BUFFER);

    return lastUnit;
  }

  osg::ref_ptr<osgPPU::Unit> unitForType(PostProcessingEffect::UnitType type)
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

      unit = getLastUnit();
      break;

    }

    return unit;
  }

  std::string postProcessingEffectName(unsigned int index)
  {
    unsigned int c = 0;

    for (PostProcessingStateDictionary::iterator it = ppeDictionary.begin(); it != ppeDictionary.end(); ++it)
    {
      if (c == index)
        return it->first;

      c++;
    }

    return "";
  }

  void initializePPU()
  {
    if (ppuInitialized)
      return;

    processor = new Processor();
    processor->setCamera(sceneCamera);

    ppGroup->addChild(processor);

    osgPPU::Camera::resizeViewport(0, 0, resolution.x(), resolution.y(), sceneCamera);
    processor->onViewportChange();

    ppuInitialized = true;
  }
};

View::View()
  : osgViewer::View()
  , m(new Impl())
{
  m->initialize(getCamera());
  //osgViewer::View::setSceneData(m->hudGroup);
  osgViewer::View::setSceneData(m->ppGroup);
}

void View::updateResolution(Vec2f resolution)
{
  m->resolution = resolution;
  m->resolutionInitialized = true;

  if (!m->fullscreenEnabled)
  {
    m->windowRect.z() = m->resolution.x();
    m->windowRect.w() = m->resolution.y();
  }

  ref_ptr<Viewport> vp = new Viewport(0, 0, int(m->resolution.x()), int(m->resolution.y()));

  if (m->processor.valid())
  {
    osgPPU::Camera::resizeViewport(0, 0, int(m->resolution.x()), int(m->resolution.y()), m->sceneCamera);
    m->processor->onViewportChange();
  }

  m->sceneCamera->setViewport(vp);
  m->hudCamera->setViewport(vp);

  m->updateCameraRenderTextures(true);
}

void View::updateWindowPosition(Vec2f position)
{
  if (!m->fullscreenEnabled)
  {
    m->windowRect.x() = position.x();
    m->windowRect.y() = position.y();
  }
}

void View::setSceneData(Node* node)
{
  if (node == m->ppSceneData.get())
    return;

  if (m->ppSceneData.valid())
    m->ppGroup->removeChild(m->ppSceneData);

  m->ppSceneData = node;

  if (m->ppSceneData.valid())
    m->ppGroup->addChild(m->ppSceneData);
}

void View::setClampColorEnabled(bool enabled)
{
  if (!m->clampColor.valid())
  {
    m->clampColor = new ClampColor();
    m->clampColor->setClampVertexColor(GL_FALSE);
    m->clampColor->setClampFragmentColor(GL_FALSE);
    m->clampColor->setClampReadColor(GL_FALSE);
  }

  m->ppGroup->getOrCreateStateSet()->setAttribute(m->clampColor, enabled ? StateAttribute::ON : StateAttribute::OFF);
}

ref_ptr<Group> View::getRootGroup()
{
  return m->ppGroup;
}

ref_ptr<Hud> View::getHud()
{
  return m->hud;
}

osg::ref_ptr<osg::Camera> View::getSceneCamera()
{
  return m->sceneCamera;
}

osg::ref_ptr<osg::Camera> View::getHudCamera()
{
  return m->hudCamera;
}

// TODO: refactor
void View::setHud(ref_ptr<Hud> hud)
{
  if (m->hud == hud)
    return;

  if (m->hud.valid())
  {
    //hudSwitch->setChildValue(hud->getProjection(), false);
    m->hudSwitch->removeChild(m->hud->getProjection());
  }

  m->hud = hud;

  /*if (!hudSwitch->containsNode(hud->getProjection()))
  {
  hudSwitch->addChild(hud->getProjection() , true);
  }
  else
  {
  hudSwitch->setChildValue(hud->getProjection(), true);
  }*/

  m->hudSwitch->addChild(m->hud->getProjection(), true);
}

void View::addPostProcessingEffect(ref_ptr<PostProcessingEffect> ppe, bool enabled, string name)
{
  if (enabled)
    m->resetPostProcessingEffects();

  // ppe->initialize();

  Impl::PostProcessingState pps;
  pps.effect = ppe;
  pps.enabled = enabled;

  if (name.empty())
    name = ppe->getName();

  m->ppeDictionary.insert(Impl::PostProcessingStateDictionary::value_type(name, pps));

  if (enabled)
    m->setupPostProcessingEffects();
}

void View::setPostProcessingEffectEnabled(string ppeName, bool enabled)
{
  Impl::PostProcessingStateDictionary::iterator it = m->ppeDictionary.find(ppeName);

  if (it->second.enabled == enabled)
    return;

  printf("Post processing effect '%s': %s\n", ppeName.c_str(), enabled ? "enabled" : "disabled");

  m->resetPostProcessingEffects();

  it->second.enabled = enabled;

  m->setupPostProcessingEffects();
}

void View::setPostProcessingEffectEnabled(unsigned int index, bool enabled)
{
  setPostProcessingEffectEnabled(m->postProcessingEffectName(index), enabled);
}

void View::setFullscreenEnabled(bool enabled, const osgViewer::ViewerBase::Windows& windows)
{
  if (m->fullscreenEnabled == enabled)
    return;

  m->fullscreenEnabled = enabled;

  m->updateWindowRect(windows);
}

void View::setWindowedResolution(Vec2f resolution, const osgViewer::ViewerBase::Windows& windows)
{
  m->windowRect.z() = resolution.x();
  m->windowRect.w() = resolution.y();

  m->updateWindowRect(windows);
}

void View::setScreenNum(int screenNum)
{
  int numScreens = int(GraphicsContext::getWindowingSystemInterface()->getNumScreens());

  if (screenNum >= numScreens)
    screenNum = numScreens - 1;

  m->screenNum = screenNum;
}

ref_ptr<PostProcessingEffect> View::getPostProcessingEffect(string ppeName)
{
  return m->ppeDictionary.find(ppeName)->second.effect;
}

ref_ptr<PostProcessingEffect> View::getPostProcessingEffect(unsigned int index)
{
  return getPostProcessingEffect(m->postProcessingEffectName(index));
}

bool View::getFullscreenEnabled()
{
  return m->fullscreenEnabled;
}

Vec2f View::getResolution()
{
  assert(m->resolutionInitialized);
  return m->resolution;
}

int View::getScreenNum()
{
  return m->screenNum;
}

bool View::getPostProcessingEffectEnabled(string ppeName)
{
  Impl::PostProcessingStateDictionary::iterator it = m->ppeDictionary.find(ppeName);

  return it->second.enabled;
}

bool View::getPostProcessingEffectEnabled(unsigned int index)
{
  return getPostProcessingEffectEnabled(m->postProcessingEffectName(index));
}

bool View::hasPostProcessingEffect(string ppeName)
{
  return m->ppeDictionary.find(ppeName) != m->ppeDictionary.end();
}

void View::setupResolution()
{
  unsigned int screenWidth, screenHeight;
  GraphicsContext::getWindowingSystemInterface()->getScreenResolution(GraphicsContext::ScreenIdentifier(m->screenNum), screenWidth, screenHeight);

  m->windowRect.x() = float(screenWidth) / 2.0f - m->windowRect.z() / 2.0f;
  m->windowRect.y() = float(screenHeight) / 2.0f - m->windowRect.w() / 2.0f;

  if (!m->fullscreenEnabled)
  {
    setUpViewInWindow(
      int(m->windowRect.x()),
      int(m->windowRect.y()),
      int(m->windowRect.z()),
      int(m->windowRect.w()),
      m->screenNum);

    m->resolution = Vec2f(m->windowRect.z(), m->windowRect.w());
  }
  else
  {
    m->resolution = Vec2f(float(screenWidth), float(screenHeight));
  }

  m->resolutionInitialized = true;
}

}