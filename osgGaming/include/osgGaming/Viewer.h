#pragma once

#include <map>

#include <osgViewer/Viewer>
#include <osg/Texture2D>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>

namespace osgGaming
{
	class Viewer : public osgViewer::Viewer
	{
	public:
		Viewer();

		void updateResolution(float width, float height);
		virtual void setSceneData(osg::Node* node) override;

		osg::ref_ptr<osg::Group> getRootGroup();

	private:
		typedef struct _renderTexture
		{
			osg::ref_ptr<osg::Texture2D> texture;
			osg::ref_ptr<osgPPU::Unit> bypassUnit;
		} RenderTexture;

		typedef std::map<osg::Camera::BufferComponent, RenderTexture> RenderTextureDictionary;

		osg::ref_ptr<osgPPU::Unit> bypassUnit(osg::Camera::BufferComponent bufferComponent);

		void initializePPU();

		bool _resolutionKnown;
		osg::Vec2f _resolution;

		osg::ref_ptr<osg::Node> _ppSceneData;

		osg::ref_ptr<osgPPU::Processor> _processor;
		osg::ref_ptr<osg::Group> _ppGroup;
		osg::ref_ptr<osgPPU::Unit> _outputUnit;

		bool _ppuInitialized;
		RenderTextureDictionary _renderTextures;
	};
}