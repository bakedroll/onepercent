#pragma once

#include <osg/Geometry>
#include <osg/Program>

#include <string.h>

using namespace osg;
using namespace std;

namespace onep
{
	class GlobeModel : public Group
	{
	public:
		GlobeModel();

	private:
		ref_ptr<Geode> createPlanetGeode(int textureResolution);

		ref_ptr<StateSet> createStateSet();
		ref_ptr<Program> createShader();
		void loadTexture(ref_ptr<StateSet> stateSet, string filename, int tex_layer, string uniform_name);
		ref_ptr<Geometry> createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice);
	};
}