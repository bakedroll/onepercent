#pragma once

#include <osg/NodeCallback>
#include <functional>

namespace osgGaming
{
	class FpsUpdateCallback : public osg::NodeCallback
	{
	public:
		FpsUpdateCallback();

    void setUpdateFunc(std::function<void(int)> func);
    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv) override;

	private:
		int m_framesCount;
		double m_lastSimulationTime;
    std::function<void(int)> m_updateFunc;

	};
}