#pragma once

#include <osg/NodeCallback>
#include <osg/NodeVisitor>

namespace osgGaming
{
	class SimulationCallback : public osg::NodeCallback
	{
	public:
	  SimulationCallback();
    virtual ~SimulationCallback();

		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override final;
		void resetTimeDiff();

	protected:
		virtual void action(osg::Node* node, osg::NodeVisitor* nv, double simTime, double timeDiff) = 0;

	private:
		double _lastSimulationTime;
		bool _resetTimeDiff;

	};
}