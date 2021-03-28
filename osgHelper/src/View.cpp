#include <osgHelper/View.h>
#include <osgHelper/LogManager.h>
#include <osgHelper/Macros.h>

#include <osg/ClampColor>
#include <osg/Texture2D>

#include <osgViewer/Renderer>

#include <osgPPU/Unit.h>
#include <osgPPU/UnitOut.h>
#include <osgPPU/UnitInOut.h>
#include <osgPPU/Processor.h>
#include <osgPPU/UnitBypass.h>
#include <osgPPU/UnitDepthbufferBypass.h>

#include <osgDB/WriteFile>

#include <cassert>

namespace osgHelper
{
  struct View::Impl
  {
    Impl()
      : sceneGraph(new osg::Group())
      , camera(new osgHelper::Camera())
      , isResolutionInitialized(false)
      , isPipelineDirty(false)
    {
    }

    struct RenderTexture
    {
        osg::ref_ptr<osg::Texture2D> texture;
        osg::ref_ptr<osgPPU::Unit>   bypassUnit;
    };

    struct PostProcessingState
    {
      osg::ref_ptr<ppu::Effect> effect;
      bool                      isEnabled    = true;
      bool                      isIntegrated = false;
    };

    using RenderTextureDictionary       = std::map<int, RenderTexture>;
    using PostProcessingStateDictionary = std::map<std::string, PostProcessingState>;

    osg::ref_ptr<osg::Group>        sceneGraph;
    osg::ref_ptr<osgHelper::Camera> camera;

    osg::Vec2f resolution;
    bool       isResolutionInitialized;
    bool       isPipelineDirty;

    osg::ref_ptr<osgPPU::Processor> processor;
    osg::ref_ptr<osg::ClampColor>   clampColor;

    osg::ref_ptr<osgPPU::Unit>      lastUnit;
    osg::ref_ptr<osgPPU::UnitOut>   unitOutput;

    PostProcessingStateDictionary ppeDictionary;
    RenderTextureDictionary       renderTextures;

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

      camera->attach(bufferComponent, texture);

      return texture;
    }

    RenderTexture getOrCreateRenderTexture(osg::Camera::BufferComponent bufferComponent,
                                           UpdateMode                   mode = UpdateMode::Keep)
    {
      auto it = renderTextures.find(bufferComponent);
      if (it != renderTextures.end())
      {
        auto& renderTexture = it->second;

        if (mode == UpdateMode::Recreate)
        {
          camera->detach(bufferComponent);
          renderTexture.texture = createRenderTexture(bufferComponent);
        }

        return renderTexture;
      }

      RenderTexture rt;
      rt.texture = createRenderTexture(bufferComponent);

      renderTextures[bufferComponent] = rt;

      return rt;
    }

    osg::ref_ptr<osgPPU::Unit> getBypassUnit(osg::Camera::BufferComponent bufferComponent)
    {
      auto renderTexture = getOrCreateRenderTexture(bufferComponent);

      if (!renderTexture.bypassUnit.valid())
      {
        switch (bufferComponent)
        {
          case osg::Camera::COLOR_BUFFER:
          {
            renderTexture.bypassUnit = new osgPPU::UnitBypass();

            //auto unit = new osgPPU::UnitCameraAttachmentBypass();
            //unit->setBufferComponent(osg::Camera::COLOR_BUFFER0);
            //renderTexture.bypassUnit = unit;

            processor->addChild(renderTexture.bypassUnit);

            break;
          }
          case osg::Camera::DEPTH_BUFFER:
          {
            renderTexture.bypassUnit = new osgPPU::UnitDepthbufferBypass();

            //auto unit = new osgPPU::UnitCameraAttachmentBypass();
            //unit->setBufferComponent(osg::Camera::DEPTH_BUFFER);
            //renderTexture.bypassUnit = unit;

            processor->addChild(renderTexture.bypassUnit);
            break;
          }
          default:
            break;
            ;
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

    osg::ref_ptr<osgPPU::Unit> unitForType(ppu::Effect::UnitType type)
    {
      switch (type)
      {
        case ppu::Effect::UnitType::BypassColor:
          return getBypassUnit(osg::Camera::COLOR_BUFFER);
        case ppu::Effect::UnitType::BypassDepth:
          return getBypassUnit(osg::Camera::DEPTH_BUFFER);
        case ppu::Effect::UnitType::OngoingColor:
          return getLastUnit();
        default:
          break;
      }

      return nullptr;
    }

  };

  View::View()
    : osgViewer::View()
    , m(new Impl())
  {
    setCamera(m->camera);
    setSceneData(m->sceneGraph);
  }

  View::~View() = default;

  void View::updateResolution(const osg::Vec2f& resolution, int pixelRatio)
  {
    const auto width  = static_cast<int>(resolution.x());
    const auto height = static_cast<int>(resolution.y());

    m->resolution              = resolution;
    m->isResolutionInitialized = true;

    if (m->isPipelineDirty)
    {
      alterPipelineState([](){}, UpdateMode::Recreate);
      m->isPipelineDirty = false;
    }

    for (auto& effect : m->ppeDictionary)
    {
      effect.second.effect->onResizeViewport(resolution);
    }

    if (m->processor.valid())
    {
      updateCameraRenderTextures(UpdateMode::Recreate);

      osgPPU::Camera::resizeViewport(0, 0, width, height, m->camera);
      m->processor->onViewportChange();

      updateViewport(0, 0, width, height, pixelRatio);

      m->processor->dirtyUnitSubgraph();
    }
  }

  void View::updateViewport(int x, int y, int width, int height, int pixelRatio)
  {
    const auto viewport = new osg::Viewport(x, y, width * pixelRatio, height * pixelRatio);
    m->camera->setViewport(viewport);
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

    m->camera->getOrCreateStateSet()->setAttribute(m->clampColor, enabled ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
  }

  osg::ref_ptr<osg::Group> View::getRootGroup() const
  {
    return m->sceneGraph;
  }

  osg::ref_ptr<osgHelper::Camera> View::getSceneCamera() const
  {
    return m->camera;
  }

  void View::addPostProcessingEffect(const osg::ref_ptr<ppu::Effect>& ppe, bool enabled, const std::string& name)
  {
    alterPipelineState([this, ppe, enabled, name]()
    {
      Impl::PostProcessingState pps;
      pps.effect  = ppe;
      pps.isEnabled = enabled;

      m->ppeDictionary[name.empty() ? ppe->getName() : name] = pps;
    }, enabled ? UpdateMode::Recreate : UpdateMode::Keep);
  }

  void View::setPostProcessingEffectEnabled(const std::string& ppeName, bool enabled)
  {
    if (m->ppeDictionary.count(ppeName) == 0)
    {
      OSGG_LOG_WARN("Post processing effect '" + ppeName + "' not found");
      assert_return(false);
    }

    auto& ppe = m->ppeDictionary[ppeName];
    if (ppe.isEnabled == enabled)
	  {
      return;
	  }

    OSGG_LOG_DEBUG("Post processing effect '" + ppeName + "': " + (enabled ? "enabled" : "disabled"));

    alterPipelineState([&ppe, enabled]()
    {
      ppe.isEnabled = enabled;
    });
  }

  osg::ref_ptr<ppu::Effect> View::getPostProcessingEffect(const std::string& ppeName) const
  {
    if (m->ppeDictionary.count(ppeName) == 0)
    {
      return nullptr;
    }

    return m->ppeDictionary[ppeName].effect;
  }

  osg::Vec2f View::getResolution() const
  {
    assert(m->isResolutionInitialized);
    return m->resolution;
  }

  bool View::getPostProcessingEffectEnabled(const std::string& ppeName) const
  {
    return (m->ppeDictionary.count(ppeName) > 0) ? m->ppeDictionary[ppeName].isEnabled : false;
  }

  bool View::hasPostProcessingEffect(const std::string& ppeName) const
  {
    return (m->ppeDictionary.count(ppeName) > 0);
  }

  void View::initializePipelineProcessor()
  {
    m->camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT, osg::Camera::FRAME_BUFFER);
    //m->camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);  //, osg::Camera::FRAME_BUFFER);
    m->camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    m->camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m->camera->setRenderOrder(osg::Camera::PRE_RENDER);

    m->processor = new osgPPU::Processor();
    m->processor->setCamera(m->camera);

    m->sceneGraph->addChild(m->processor);

    osgPPU::Camera::resizeViewport(0, 0, m->resolution.x(), m->resolution.y(), m->camera);
    m->processor->onViewportChange();
  }

  void View::assemblePipeline()
  {
    m->lastUnit = m->getLastUnit(true);

    for (auto& it : m->ppeDictionary)
    {
      it.second.isIntegrated = it.second.isEnabled;

      if (!it.second.isEnabled)
      {
        continue;
      }

      const auto& ppe = it.second.effect;

      ppe->initialize();

      auto initialUnits = ppe->getInitialUnits();
      for (const auto& unit : initialUnits)
      {
        m->unitForType(unit.type)->addChild(unit.unit);
      }

      auto inputToUniformList = ppe->getInputToUniform();
      for (const auto& itou : inputToUniformList)
      {
        itou.unit->setInputToUniform(m->unitForType(itou.type), itou.name, true);
      }

      m->lastUnit = ppe->getResultUnit();
    }

    m->lastUnit->addChild(m->unitOutput);

    updateCameraRenderTextures();
    m->processor->dirtyUnitSubgraph();
  }

  void View::disassemblePipeline()
  {
    if (!m->processor.valid())
    {
      initializePipelineProcessor();
    }

    m->lastUnit = m->getLastUnit(true);

    for (auto& it : m->ppeDictionary)
    {
        if (!it.second.isEnabled || !it.second.isIntegrated)
        {
            continue;
        }

        auto& ppe = it.second.effect;

        auto initialUnits = ppe->getInitialUnits();
        for (const auto& unit : initialUnits)
        {
            m->unitForType(unit.type)->removeChild(unit.unit);
        }

        auto inputToUniformList = ppe->getInputToUniform();
        for (const auto& itou : inputToUniformList)
        {
            m->unitForType(itou.type)->removeChild(itou.unit);
        }

        m->lastUnit            = ppe->getResultUnit();
        it.second.isIntegrated = false;
    }

    if (m->unitOutput.valid())
    {
        m->lastUnit->removeChild(m->unitOutput);
    }

    //auto width  = static_cast<int>(m->resolution.x());
    //auto height = static_cast<int>(m->resolution.y());

    m->unitOutput = new osgPPU::UnitOut();
    m->unitOutput->setInputTextureIndexForViewportReference(-1);
    m->unitOutput->setViewport(m->camera->getViewport());
           // new osg::Viewport(0, 0, width, height));

    //m->unitOutput = new osgPPU::UnitOutCapture();
    //m->unitOutput->setPath("./");
    //m->unitOutput->setFileExtension("bmp");

  }

  void View::alterPipelineState(const std::function<void()>& func, UpdateMode mode)
  {
    const auto canSetupPipeline = (m->isResolutionInitialized && (mode == UpdateMode::Recreate));
    m->isPipelineDirty          = (!m->isResolutionInitialized && (mode == UpdateMode::Recreate));

    if (!canSetupPipeline)
    {
      func();
      return;
    }

    disassemblePipeline();
    func();
    assemblePipeline();

    updateCameraRenderTextures();
    m->processor->dirtyUnitSubgraph();
  }

  void View::updateCameraRenderTextures(UpdateMode mode)
  {
    if (mode == UpdateMode::Recreate)
    {
      for (const auto& it : m->renderTextures)
      {
        m->getOrCreateRenderTexture(static_cast<osg::Camera::BufferComponent>(it.first), mode);
      }
    }

    auto renderer = dynamic_cast<osgViewer::Renderer*>(m->camera->getRenderer());
    renderer->getSceneView(0)->getRenderStage()->setCameraRequiresSetUp(true);
    renderer->getSceneView(0)->getRenderStage()->setFrameBufferObject(nullptr);
  }
}  // namespace osgHelper
