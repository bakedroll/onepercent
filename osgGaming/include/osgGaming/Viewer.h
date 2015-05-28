#pragma once

#include <map>

#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/ClampColor>
#include <osg/Camera>
#include <osg/Switch>
#include <osg/StateSet>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>

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

		void addPostProcessingEffect(osg::ref_ptr<PostProcessingEffect> ppe);

	private:
		typedef struct _renderTexture
		{
			osg::ref_ptr<osg::Texture2D> texture;
			osg::ref_ptr<osgPPU::Unit> bypassUnit;
		} RenderTexture;

		typedef std::map<int, RenderTexture> RenderTextureDictionary;
		typedef std::vector<osg::ref_ptr<PostProcessingEffect>> PostProcessingEffectList;

		void initialize();

		RenderTexture renderTexture(osg::Camera::BufferComponent bufferComponent, bool recreate = false);
		osg::ref_ptr<osg::Texture2D> createRenderTexture(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> bypassUnit(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> lastUnit();

		osg::ref_ptr<osgPPU::Unit> unitForType(PostProcessingEffect::UnitType type);

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

		bool _ppuInitialized;
		RenderTextureDictionary _renderTextures;
		PostProcessingEffectList _ppeList;
	};
}