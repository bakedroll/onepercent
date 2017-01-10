#pragma once

#include <map>

#include <osgViewer/View>
#include <osg/Texture2D>
#include <osg/ClampColor>
#include <osg/Camera>
#include <osg/Switch>
#include <osg/StateSet>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>
#include <osgPPU/UnitInOut.h>

#include <osgGaming/PostProcessingEffect.h>
#include <osgGaming/Hud.h>

namespace osgGaming
{
	class View : public osgViewer::View
	{
	public:
		View();

		void updateResolution(osg::Vec2f resolution);
		void updateWindowPosition(osg::Vec2f position);
		virtual void setSceneData(osg::Node* node) override;

		void setClampColorEnabled(bool enabled);

		osg::ref_ptr<osg::Group> getRootGroup();
		osg::ref_ptr<Hud> getHud();
    osg::ref_ptr<osg::Camera> getHudCamera();

		void setHud(osg::ref_ptr<Hud> hud);

		void addPostProcessingEffect(osg::ref_ptr<PostProcessingEffect> ppe, bool enabled = true, std::string name = "");

		void setPostProcessingEffectEnabled(std::string ppeName, bool enabled);
		void setPostProcessingEffectEnabled(unsigned int index, bool enabled);
    void setFullscreenEnabled(bool enabled, const osgViewer::ViewerBase::Windows& windows);
    void setWindowedResolution(osg::Vec2f resolution, const osgViewer::ViewerBase::Windows& windows);
		void setScreenNum(int screenNum);

		osg::ref_ptr<PostProcessingEffect> getPostProcessingEffect(std::string ppeName);
		osg::ref_ptr<PostProcessingEffect> getPostProcessingEffect(unsigned int index);
		bool getFullscreenEnabled();
		osg::Vec2f getResolution();
		int getScreenNum();

		bool getPostProcessingEffectEnabled(std::string ppeName);
		bool getPostProcessingEffectEnabled(unsigned int index);

		bool hasPostProcessingEffect(std::string ppeName);

		void setupResolution();

	private:
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

		void initialize();

		void resetPostProcessingEffects();
		void setupPostProcessingEffects();

		void updateCameraRenderTextures(bool recreate = false);
    void updateWindowRect(const osgViewer::ViewerBase::Windows& windows);

		RenderTexture renderTexture(osg::Camera::BufferComponent bufferComponent, bool recreate = false);
		osg::ref_ptr<osg::Texture2D> createRenderTexture(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> bypassUnit(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> lastUnit(bool reset = false);

		osg::ref_ptr<osgPPU::Unit> unitForType(PostProcessingEffect::UnitType type);

		std::string postProcessingEffectName(unsigned int index);

		void initializePPU();

		osg::Vec2f _resolution;
		osg::Vec4f _windowRect;
		bool _fullscreenEnabled;
		bool _resolutionInitialized;
		int _screenNum;

		osg::ref_ptr<osg::Node> _ppSceneData;
		osg::ref_ptr<osg::StateSet> _hudStateSet;
		osg::ref_ptr<osg::Camera> _hudCamera;
		osg::ref_ptr<osg::Switch> _hudSwitch;
		osg::ref_ptr<Hud> _hud;

		osg::ref_ptr<osgPPU::Processor> _processor;
		osg::ref_ptr<osg::Group> _ppGroup;
		osg::ref_ptr<osg::ClampColor> _clampColor;

		osg::ref_ptr<osgPPU::Unit> _lastUnit;
		osg::ref_ptr<osgPPU::UnitInOut> _unitOutput;

		bool _ppuInitialized;
		RenderTextureDictionary _renderTextures;
		PostProcessingStateDictionary _ppeDictionary;
	};
}