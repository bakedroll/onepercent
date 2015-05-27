#include <osgGaming/SimulationCallback.h>

using namespace osg;
using namespace osgGaming;

SimulationCallback::SimulationCallback()
	: NodeCallback(),
	  _lastSimulationTime(0.0),
	  _resetTimeDiff(false)
{

}

void SimulationCallback::operator()(Node* node, NodeVisitor* nv)
{
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

	action(node, nv, time, time_diff);
}

void SimulationCallback::resetTimeDiff()
{
	_resetTimeDiff = true;
}