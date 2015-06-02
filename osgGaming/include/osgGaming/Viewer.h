#pragma once

#include <map>
#include <string>

#include <osgViewer/Viewer>
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
	class Viewer : public osgViewer::Viewer
	{
	public:
		Viewer();

		void updateResolution(float width, float height);
		virtual void setSceneData(osg::Node* node) override;

		void setClampColorEnabled(bool enabled);

		osg::ref_ptr<osg::Group> getRootGroup();
		osg::ref_ptr<Hud> getHud();

		void setHud(osg::ref_ptr<Hud> hud);

		void addPostProcessingEffect(osg::ref_ptr<PostProcessingEffect> ppe, bool enabled = true, std::string name = "");

		void setPostProcessingEffectEnabled(std::string ppeName, bool enabled);
		void setPostProcessingEffectEnabled(unsigned int index, bool enabled);

		bool getPostProcessingEffectEnabled(std::string ppeName);
		bool getPostProcessingEffectEnabled(unsigned int index);

		bool hasPostProcessingEffect(std::string ppeName);

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

		RenderTexture renderTexture(osg::Camera::BufferComponent bufferComponent, bool recreate = false);
		osg::ref_ptr<osg::Texture2D> createRenderTexture(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> bypassUnit(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> lastUnit(bool reset = false);

		osg::ref_ptr<osgPPU::Unit> unitForType(PostProcessingEffect::UnitType type);

		std::string postProcessingEffectName(unsigned int index);

		void initializePPU();

		bool _resolutionKnown;
		osg::Vec2f _resolution;

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