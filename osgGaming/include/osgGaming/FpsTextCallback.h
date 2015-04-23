#pragma once

#include <osg/Drawable>

namespace osgGaming
{
	class FpsTextCallback : public osg::Drawable::UpdateCallback
	{
	public:
		FpsTextCallback();

		virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable) override;

	private:
		int _framesCount;
		double _lastSimulationTime;
	};
}