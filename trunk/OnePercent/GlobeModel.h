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
		ref_ptr<Program> createShader();
	};
}