#include "GlobeOverviewWorld.h"
#include "GlobeModel.h"

#include <osgGaming/Helper.h>

using namespace onep;
using namespace osg;
using namespace osgGaming;

ref_ptr<PositionAttitudeTransform> GlobeOverviewWorld::getGlobeTransform()
{
	return _globeTransform;
}

void GlobeOverviewWorld::setGlobeTransform(ref_ptr<PositionAttitudeTransform> transform)
{
	if (_globeTransform.valid())
	{
		getRootNode()->removeChild(_globeTransform);
	}

	_globeTransform = transform;
	getRootNode()->addChild(_globeTransform);
}