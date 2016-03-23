#pragma once

#include <osgGaming/UIElement.h>

#include <osg/NodeVisitor>

namespace osgGaming
{
	class UIFindElementVisitor : public osg::NodeVisitor
	{
	public:
		UIFindElementVisitor(std::string name);

		virtual void apply(osg::Node &node) override;

		osg::ref_ptr<UIElement> getResult();

	private:
		std::string _name;
		osg::ref_ptr<UIElement> _result;

	};
}