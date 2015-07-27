#include <osgGaming/UserInteractionModel.h>

#include <osgGaming/UIElement.h>

using namespace osgGaming;
using namespace std;

UserInteractionModel::UserInteractionModel()
	: _hovered(false)
{

}

bool UserInteractionModel::getHovered()
{
	return _hovered;
}

/*string UserInteractionModel::getUIMName()
{
	return dynamic_cast<UIElement*>(this)->getUIName();
}*/

void UserInteractionModel::setHovered(bool hovered)
{
	_hovered = hovered;
}

/*void UserInteractionModel::setUIMName(string name)
{
	_name = name;
}
*/