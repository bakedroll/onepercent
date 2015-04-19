#pragma once

#include <osg/Geometry>
#include <osg/Program>

#include <string.h>

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
		GlobeModel();

	private:
		osg::ref_ptr<osg::Geode> createPlanetGeode(int textureResolution);

		osg::ref_ptr<osg::StateSet> createStateSet();
		osg::ref_ptr<osg::Program> createShader();
		void loadTexture(osg::ref_ptr<osg::StateSet> stateSet, std::string filename, int tex_layer, std::string uniform_name);
		osg::ref_ptr<osg::Geometry> createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice);
	};
}