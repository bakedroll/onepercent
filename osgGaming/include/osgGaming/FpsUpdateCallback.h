#pragma once

#include <osg/Callback>
#include <functional>

namespace osgGaming
{
	class FpsUpdateCallback : public osg::Callback
	{
	public:
		FpsUpdateCallback();

    void setUpdateFunc(std::function<void(int)> func);
    bool run(osg::Object* node, osg::Object* data) override;

	private:
		int m_framesCount;
		double m_lastSimulationTime;
    std::function<void(int)> m_updateFunc;

	};
}