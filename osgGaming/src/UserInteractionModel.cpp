#include <osgGaming/UserInteractionModel.h>

using namespace osgGaming;
using namespace std;

UserInteractionModel::UserInteractionModel()
	: _hovered(false),
	  _name("")
{

}

bool UserInteractionModel::getHovered()
{
	return _hovered;
}

string UserInteractionModel::getUIMName()
{
	return _name;
}

void UserInteractionModel::setHovered(bool hovered)
{
	_hovered = hovered;
}

void UserInteractionModel::setUIMName(string name)
{
	_name = name;
}