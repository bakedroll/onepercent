#include <osgGaming/UIFindElementVisitor.h>

using namespace osgGaming;
using namespace osg;
using namespace std;

UIFindElementVisitor::UIFindElementVisitor(string name)
	: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN),
	  _name(name)
{

}

void UIFindElementVisitor::apply(osg::Node &node)
{
	ref_ptr<UIElement> uiElement = dynamic_cast<UIElement*>(&node);

	if (uiElement.valid() && uiElement->getUIName() == _name)
	{
		_result = uiElement;
		return;
	}

	traverse(node);
}

ref_ptr<UIElement> UIFindElementVisitor::getResult()
{
	return _result;
}