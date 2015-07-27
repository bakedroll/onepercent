#pragma once

#include <vector>

#include <osgGaming/UserInteractionModel.h>

#include <osg/NodeVisitor>

namespace osgGaming
{
	class UIMCollectorVisitor : public osg::NodeVisitor
	{
	public:
		UIMCollectorVisitor();

		virtual void apply(osg::Node &node) override;

		UserInteractionModelList getUserInteractionModels();

	private:
		UserInteractionModelList _uimList;
	};
}