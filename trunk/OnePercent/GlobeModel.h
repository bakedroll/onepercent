#pragma once

#include <osg/Geometry>
#include <osg/Program>

using namespace osg;

namespace onep
{
	class GlobeModel : public Group
	{
	public:
		GlobeModel();

	private:
		ref_ptr<StateSet> createStateSet();
		ref_ptr<Program> createShader();
		ref_ptr<Geode> createMesh(int stacks = 12, int slices = 24, double radius = 1.0);
	};
}