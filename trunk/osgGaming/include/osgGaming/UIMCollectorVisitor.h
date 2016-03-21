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

		UserInteractionModel::List getUserInteractionModels();

	private:
		UserInteractionModel::List _uimList;
	};
}