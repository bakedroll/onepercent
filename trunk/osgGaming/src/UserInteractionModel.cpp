#include <osgGaming/UserInteractionModel.h>

using namespace osgGaming;

UserInteractionModel::UserInteractionModel()
	: _hovered(false)
{

}

bool UserInteractionModel::getHovered()
{
	return _hovered;
}

void UserInteractionModel::setHovered(bool hovered)
{
	_hovered = hovered;
}