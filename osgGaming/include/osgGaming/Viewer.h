#pragma once

#include <map>

#include <osgViewer/Viewer>
#include <osg/Texture2D>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>

#include <osgGaming/PostProcessingEffect.h>

namespace osgGaming
{
	class Viewer : public osgViewer::Viewer
	{
	public:
		Viewer();

		void updateResolution(float width, float height);
		virtual void setSceneData(osg::Node* node) override;

		osg::ref_ptr<osg::Group> getRootGroup();

		void addPostProcessingEffect(osg::ref_ptr<PostProcessingEffect> ppe);

	private:
		typedef struct _renderTexture
		{
			osg::ref_ptr<osg::Texture2D> texture;
			osg::ref_ptr<osgPPU::Unit> bypassUnit;
		} RenderTexture;

		typedef std::map<osg::Camera::BufferComponent, RenderTexture> RenderTextureDictionary;
		typedef std::vector<osg::ref_ptr<PostProcessingEffect>> PostProcessingEffectList;

		osg::ref_ptr<osgPPU::Unit> bypassUnit(osg::Camera::BufferComponent bufferComponent);
		osg::ref_ptr<osgPPU::Unit> lastUnit();

		osg::ref_ptr<osgPPU::Unit> unitForType(PostProcessingEffect::UnitType type);

		void initializePPU();

		bool _resolutionKnown;
		osg::Vec2f _resolution;

		osg::ref_ptr<osg::Node> _ppSceneData;

		osg::ref_ptr<osgPPU::Processor> _processor;
		osg::ref_ptr<osg::Group> _ppGroup;
		//osg::ref_ptr<osgPPU::Unit> _outputUnit;

		osg::ref_ptr<osgPPU::Unit> _lastUnit;

		bool _ppuInitialized;
		RenderTextureDictionary _renderTextures;
		PostProcessingEffectList _ppeList;
	};
}