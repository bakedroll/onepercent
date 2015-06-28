#include <osgGaming/UIContainerElementBase.h>

using namespace osgGaming;
using namespace osg;

UIContainerElementBase::UIContainerElementBase()
	: UIElement()
{

}

UIContainerElementBase::UIElementList UIContainerElementBase::getUIChildren()
{
	UIElementList elements;

	unsigned int num = getChildGroup()->getNumChildren();
	for (unsigned int i = 0; i < num; i++)
	{
		ref_ptr<UIElement> element = dynamic_cast<UIElement*>(getChildGroup()->getChild(i));
		if (element.valid())
		{
			elements.push_back(element);
		}
	}

	return elements;
}

unsigned int UIContainerElementBase::getNumUIChildren()
{
	unsigned int result = 0;

	unsigned int num = getChildGroup()->getNumChildren();
	for (unsigned int i = 0; i < num; i++)
	{
		ref_ptr<UIElement> element = dynamic_cast<UIElement*>(getChildGroup()->getChild(i));
		if (element.valid())
		{
			result++;
		}
	}

	return result;
}

void UIContainerElementBase::onResetMinContentSize()
{
	UIElementList children = getUIChildren();

	for (UIElementList::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->resetMinContentSize();
	}
}