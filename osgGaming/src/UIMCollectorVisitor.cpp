#include <osgGaming/UIMCollectorVisitor.h>

using namespace osgGaming;
using namespace osg;

UIMCollectorVisitor::UIMCollectorVisitor()
	: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
{

}

void UIMCollectorVisitor::apply(Node &node)
{
	UserInteractionModel* uim = dynamic_cast<UserInteractionModel*>(&node);

	if (uim != NULL)
	{
		_uimList.push_back(uim);
	}

	traverse(node);
}

UserInteractionModelList UIMCollectorVisitor::getUserInteractionModels()
{
	return _uimList;
}