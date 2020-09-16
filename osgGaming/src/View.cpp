#include "osgGaming/View.h"
#include "osgGaming/LogManager.h"
#include "osgGaming/Macros.h"

#include <osg/ClampColor>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <osgViewer/Renderer>

#include <osgPPU/Unit.h>
#include <osgPPU/UnitInOut.h>
#include <osgPPU/Processor.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitDepthbufferBypass.h>

#include <cassert>

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

    //! Determines if the render texture should be recreated
    enum class UpdateTextureMode
    {
      Recreate, //!< recreate
      Keep      //!< do not recreate
    };

    struct RenderTexture
    {
      osg::ref_ptr<osg::Texture2D> texture;
      osg::ref_ptr<osgPPU::Unit>   bypassUnit;
    };

    struct PostProcessingState
    {
      osg::ref_ptr<PostProcessingEffect> effect;
      bool                               enabled;
    };

    using RenderTextureDictionary       = std::map<int, RenderTexture>;
    using PostProcessingStateDictionary = std::map<std::string, PostProcessingState>;

    osg::Vec2f resolution;
    osg::Vec4f windowRect;
    bool       fullscreenEnabled;
    bool       resolutionInitialized;
    int        screenNum;

    osg::ref_ptr<osg::Node>     ppSceneData;
    osg::ref_ptr<osg::StateSet> hudStateSet;
    osg::ref_ptr<osg::Camera>   sceneCamera;
    osg::ref_ptr<osg::Camera>   hudCamera;
    osg::ref_ptr<osg::Switch>   hudSwitch;
    osg::ref_ptr<Hud>           hud;

    osg::ref_ptr<osg::Geode> canvasGeode;

    osg::ref_ptr<osgPPU::Processor> processor;
    osg::ref_ptr<osg::Group>        ppGroup;
    osg::ref_ptr<osg::ClampColor>   clampColor;

    osg::ref_ptr<osgPPU::Unit>      lastUnit;
    osg::ref_ptr<osgPPU::UnitInOut> unitOutput;

    bool                          ppuInitialized;
    RenderTextureDictionary       renderTextures;
    PostProcessingStateDictionary ppeDictionary;

    void initialize(const osg::ref_ptr<osg::Camera>& camera)
    {
      sceneCamera = camera;
      sceneCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT, osg::Camera::FRAME_BUFFER);
      sceneCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
      sceneCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      sceneCamera->setRenderOrder(osg::Camera::PRE_RENDER);

      hudCamera = new osg::Camera();
      hudCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER, osg::Camera::FRAME_BUFFER);
      hudCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
      hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
      hudCamera->setViewMatrix(osg::Matrixd::identity());
      hudCamera->setProjectionMatrix(osg::Matrixd::identity());

      ppGroup = new osg::Group();

      // implicitly attaches texture to camera
      osg::ref_ptr<osg::Texture2D> texture = getRenderTexture(osg::Camera::COLOR_BUFFER).texture;

      canvasGeode = new osg::Geode();
      canvasGeode->addDrawable(osg::createTexturedQuadGeometry(
              osg::Vec3f(-1.0f, -1.0f, 0.0f), osg::Vec3f(2.0f, 0.0f, 0.0f), osg::Vec3f(0.0f, 2.0f, 0.0f)));

      hudStateSet = canvasGeode->getOrCreateStateSet();
      hudStateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
      hudStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

      /*osg::ref_ptr<osg::MatrixTransform> geodeTransform = new osg::MatrixTransform();
      geodeTransform->setMatrix(osg::Matrix::identity());
      geodeTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      geodeTransform->addChild(canvasGeode);*/

      hudCamera->addChild(canvasGeode);

      hudSwitch = new osg::Switch();
      hudCamera->addChild(hudSwitch);

      ppGroup->addChild(hudCamera);
    }

    void resetPostProcessingEffects()
    {
      initializePPU();

      lastUnit = getLastUnit(true);

      for (const auto& it : ppeDictionary)
      {
        if (!it.second.enabled)
        {
          continue;
        }

        auto& ppe = it.second.effect;

        auto initialUnits = ppe->getInitialUnits();
        for (const auto& unit : initialUnits)
        {
          unitForType(unit.type)->removeChild(unit.unit);
        }

        auto inputToUniformList = ppe->getInputToUniform();
        for (const auto& itou : inputToUniformList)
        {
          unitForType(itou.type)->removeChild(itou.unit);
        }

        lastUnit = ppe->getResultUnit();
      }

      if (unitOutput.valid())
      {
        lastUnit->removeChild(unitOutput);
      }

      unitOutput = new osgPPU::UnitInOut();
      unitOutput->setInputTextureIndexForViewportReference(-1);
      hudStateSet->setTextureAttributeAndModes(0, unitOutput->getOrCreateOutputTexture(0), osg::StateAttribute::ON);
    }

    void setupPostProcessingEffects()
    {
      lastUnit = getLastUnit(true);

      for (const auto& it : ppeDictionary)
      {
        if (!it.second.enabled)
		    {
          continue;
		    }

        const auto& ppe = it.second.effect;

        ppe->initialize();

        auto initialUnits = ppe->getInitialUnits();
        for (const auto& unit : initialUnits)
        {
          unitForType(unit.type)->addChild(unit.unit);
        }

        auto inputToUniformList = ppe->getInputToUniform();
        for (const auto& itou : inputToUniformList)
        {
          itou.unit->setInputToUniform(unitForType(itou.type), itou.name, true);
        }

        lastUnit = ppe->getResultUnit();
      }

      lastUnit->addChild(unitOutput);

      updateCameraRenderTextures();
      processor->dirtyUnitSubgraph();
    }

    void updateWindowRect(const osgViewer::ViewerBase::Windows& windows)
    {
      if (!resolutionInitialized)
      {
        return;
	    }

      osg::ref_ptr<osgViewer::GraphicsWindow> graphicsWindow = *windows.begin();

      if (!graphicsWindow.valid())
      {
        return;
	    }

      if (fullscreenEnabled)
      {
        unsigned int screenWidth, screenHeight;
        osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(screenNum), screenWidth, screenHeight);

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
    }

    void updateCameraRenderTextures(UpdateTextureMode mode = UpdateTextureMode::Keep)
    {
      for (const auto& it : renderTextures)
      {
        auto tex = getRenderTexture(static_cast<osg::Camera::BufferComponent>(it.first), mode).texture;
        if ((it.first == osg::Camera::COLOR_BUFFER) && !processor.valid())
        {
          hudStateSet->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
        }
      }

      auto renderer = dynamic_cast<osgViewer::Renderer*>(sceneCamera->getRenderer());
      renderer->getSceneView(0)->getRenderStage()->setCameraRequiresSetUp(true);
      renderer->getSceneView(0)->getRenderStage()->setFrameBufferObject(nullptr);
    }

    RenderTexture getRenderTexture(osg::Camera::BufferComponent bufferComponent,
                                   UpdateTextureMode            mode = UpdateTextureMode::Keep)
    {
      auto it = renderTextures.find(bufferComponent);
      if (it != renderTextures.end())
      {
        auto& renderTexture = it->second;

        if (mode == UpdateTextureMode::Recreate)
        {
          sceneCamera->detach(bufferComponent);
          renderTexture.texture = createRenderTexture(bufferComponent);
        }

        return renderTexture;
      }

      RenderTexture rt;
      rt.texture = createRenderTexture(bufferComponent);

      renderTextures[bufferComponent] = rt;

      return rt;
    }

    osg::ref_ptr<osg::Texture2D> createRenderTexture(osg::Camera::BufferComponent bufferComponent)
    {
      auto texture = new osg::Texture2D();

      texture->setTextureSize(static_cast<int>(resolution.x()), static_cast<int>(resolution.y()));
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

      default:
        break;
      }

      sceneCamera->attach(bufferComponent, texture);

      return texture;
    }

    osg::ref_ptr<osgPPU::Unit> getBypassUnit(osg::Camera::BufferComponent bufferComponent)
    {
      auto renderTexture = getRenderTexture(bufferComponent);

      if (!renderTexture.bypassUnit.valid())
      {
        switch (bufferComponent)
        {
        case osg::Camera::COLOR_BUFFER:
          renderTexture.bypassUnit = new osgPPU::UnitBypass();
          processor->addChild(renderTexture.bypassUnit);
          break;

        case osg::Camera::DEPTH_BUFFER:
          renderTexture.bypassUnit = new osgPPU::UnitDepthbufferBypass();
          processor->addChild(renderTexture.bypassUnit);
          break;

        default:
          break;;
        }

        renderTextures[bufferComponent] = renderTexture;
      }

      return renderTexture.bypassUnit;
    }

    osg::ref_ptr<osgPPU::Unit> getLastUnit(bool reset = false)
    {
      if (!lastUnit.valid() || reset)
	    {
        lastUnit = getBypassUnit(osg::Camera::COLOR_BUFFER);
	    }

      return lastUnit;
    }

    osg::ref_ptr<osgPPU::Unit> unitForType(PostProcessingEffect::UnitType type)
    {
      switch (type)
      {
	    case PostProcessingEffect::UnitType::BypassColor:
        return getBypassUnit(osg::Camera::COLOR_BUFFER);
      case PostProcessingEffect::UnitType::BypassDepth:
        return getBypassUnit(osg::Camera::DEPTH_BUFFER);
      case PostProcessingEffect::UnitType::OngoingColor:
        return getLastUnit();
	    default:
        break;
      }

      return nullptr;
    }

    void initializePPU()
    {
      if (ppuInitialized)
	    {
        return;
	    }

      processor = new osgPPU::Processor();
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
    setSceneData(m->ppGroup);
  }

  View::~View() = default;

  void View::updateResolution(const osg::Vec2f& resolution)
  {
    m->resolution            = resolution;
    m->resolutionInitialized = true;

    if (!m->fullscreenEnabled)
    {
      m->windowRect.z() = m->resolution.x();
      m->windowRect.w() = m->resolution.y();
    }

    auto viewport = new osg::Viewport(0, 0, static_cast<int>(m->resolution.x()), static_cast<int>(m->resolution.y()));

    if (m->processor.valid())
    {
      osgPPU::Camera::resizeViewport(0, 0, static_cast<int>(m->resolution.x()), static_cast<int>(m->resolution.y()),
                                     m->sceneCamera);
      m->processor->onViewportChange();
    }

    m->sceneCamera->setViewport(viewport);
    m->hudCamera->setViewport(viewport);

    m->updateCameraRenderTextures(Impl::UpdateTextureMode::Recreate);
  }

  void View::updateWindowPosition(const osg::Vec2f& position)
  {
    if (!m->fullscreenEnabled)
    {
      m->windowRect.x() = position.x();
      m->windowRect.y() = position.y();
    }
  }

  void View::setRootNode(const osg::ref_ptr<osg::Node>& node)
  {
    if (node == m->ppSceneData)
	  {
      return;
	  }

    if (m->ppSceneData.valid())
	  {
      m->ppGroup->removeChild(m->ppSceneData);
	  }

    m->ppSceneData = node;

    if (m->ppSceneData.valid())
	  {
      m->ppGroup->addChild(m->ppSceneData);
	  }
  }

  void View::setClampColorEnabled(bool enabled)
  {
    if (!m->clampColor.valid())
    {
      m->clampColor = new osg::ClampColor();
      m->clampColor->setClampVertexColor(GL_FALSE);
      m->clampColor->setClampFragmentColor(GL_FALSE);
      m->clampColor->setClampReadColor(GL_FALSE);
    }

    m->ppGroup->getOrCreateStateSet()->setAttribute(m->clampColor, enabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
  }

  osg::ref_ptr<osg::Group> View::getRootGroup() const
  {
    return m->ppGroup;
  }

  osg::ref_ptr<Hud> View::getHud() const
  {
    return m->hud;
  }

  osg::ref_ptr<osg::Camera> View::getSceneCamera() const
  {
    return m->sceneCamera;
  }

  osg::ref_ptr<osg::Camera> View::getHudCamera() const
  {
    return m->hudCamera;
  }

  osg::ref_ptr<osg::Geode> View::getCanvasGeode() const
  {
    return m->canvasGeode;
  }

  // TODO: refactor
  void View::setHud(const osg::ref_ptr<Hud>& hud)
  {
    if (m->hud == hud)
	  {
      return;
	  }

    if (m->hud.valid())
    {
      m->hudSwitch->removeChild(m->hud->getProjection());
    }

    m->hud = hud;
    m->hudSwitch->addChild(m->hud->getProjection(), true);
  }

  void View::addPostProcessingEffect(const osg::ref_ptr<PostProcessingEffect>& ppe, bool enabled,
                                     const std::string& name)
  {
    if (enabled)
	  {
      m->resetPostProcessingEffects();
	  }

    Impl::PostProcessingState pps;
    pps.effect  = ppe;
    pps.enabled = enabled;

    m->ppeDictionary[name.empty() ? ppe->getName() : name] = pps;

    if (enabled)
	  {
      m->setupPostProcessingEffects();
	  }
  }

  void View::setPostProcessingEffectEnabled(const std::string& ppeName, bool enabled)
  {
    if (m->ppeDictionary.count(ppeName) == 0)
    {
      OSGG_LOG_WARN("Post processing effect '" + ppeName + "' not found");
      assert_return(false);
    }

    auto& ppe = m->ppeDictionary[ppeName];
    if (ppe.enabled == enabled)
	  {
      return;
	  }

    OSGG_LOG_DEBUG("Post processing effect '" + ppeName + "': " + (enabled ? "enabled" : "disabled"));

    m->resetPostProcessingEffects();
    ppe.enabled = enabled;
    m->setupPostProcessingEffects();
  }

  void View::setFullscreenEnabled(bool enabled, const osgViewer::ViewerBase::Windows& windows)
  {
    if (m->fullscreenEnabled == enabled)
	  {
      return;
	  }

    m->fullscreenEnabled = enabled;
    m->updateWindowRect(windows);
  }

  void View::setWindowedResolution(const osg::Vec2f& resolution, const osgViewer::ViewerBase::Windows& windows)
  {
    m->windowRect.z() = resolution.x();
    m->windowRect.w() = resolution.y();

    m->updateWindowRect(windows);
  }

  void View::setScreenNum(int screenNum)
  {
    auto numScreens = static_cast<int>(osg::GraphicsContext::getWindowingSystemInterface()->getNumScreens());

    if (screenNum >= numScreens)
	  {
      screenNum = numScreens - 1;
	  }

    m->screenNum = screenNum;
  }

  osg::ref_ptr<PostProcessingEffect> View::getPostProcessingEffect(const std::string& ppeName) const
  {
    if (m->ppeDictionary.count(ppeName) == 0)
    {
      return nullptr;
    }

    return m->ppeDictionary[ppeName].effect;
  }

  bool View::getFullscreenEnabled() const
  {
    return m->fullscreenEnabled;
  }

  osg::Vec2f View::getResolution() const
  {
    assert(m->resolutionInitialized);
    return m->resolution;
  }

  int View::getScreenNum() const
  {
    return m->screenNum;
  }

  bool View::getPostProcessingEffectEnabled(const std::string& ppeName) const
  {
    return (m->ppeDictionary.count(ppeName) > 0) ? m->ppeDictionary[ppeName].enabled : false;
  }

  bool View::hasPostProcessingEffect(const std::string& ppeName) const
  {
    return (m->ppeDictionary.count(ppeName) > 0);
  }

  void View::setupResolution()
  {
    unsigned int screenWidth, screenHeight;
    osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(m->screenNum), screenWidth, screenHeight);

    m->windowRect.x() = (static_cast<float>(screenWidth) / 2.0f) - (m->windowRect.z() / 2.0f);
    m->windowRect.y() = (static_cast<float>(screenHeight) / 2.0f) - (m->windowRect.w() / 2.0f);

    if (!m->fullscreenEnabled)
    {
      setUpViewInWindow(static_cast<int>(m->windowRect.x()), static_cast<int>(m->windowRect.y()),
                        static_cast<int>(m->windowRect.z()), static_cast<int>(m->windowRect.w()), m->screenNum);

      m->resolution = osg::Vec2f(m->windowRect.z(), m->windowRect.w());
    }
    else
    {
      m->resolution = osg::Vec2f(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
    }

    m->resolutionInitialized = true;
  }
}
