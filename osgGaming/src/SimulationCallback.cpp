#include <osgGaming/SimulationCallback.h>

using namespace osg;
using namespace osgGaming;

SimulationCallback::SimulationCallback()
	: osg::Callback(),
	  _lastSimulationTime(0.0),
	  _resetTimeDiff(false)
{

}

SimulationCallback::~SimulationCallback()
{
}

bool SimulationCallback::run(osg::Object* node, osg::Object* data)
{
  const auto nv = dynamic_cast<osg::NodeVisitor*>(data);
  if (!nv)
  {
    return false;
  }

	double time = nv->getFrameStamp()->getSimulationTime();
	double time_diff = 0.0;

	if (_resetTimeDiff == true)
	{
		_resetTimeDiff = false;
	}
	else if (_lastSimulationTime > 0.0)
	{
		time_diff = time - _lastSimulationTime;
	}

	_lastSimulationTime = time;

	action(node, data, time, time_diff);

  return true;
}

void SimulationCallback::resetTimeDiff()
{
	_resetTimeDiff = true;
}