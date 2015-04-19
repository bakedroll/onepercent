#include <osgGaming/GameException.h>

using namespace osgGaming;

GameException::GameException(std::string message)
	: exception(message.c_str()),
	  _message(message)
{

}

GameException::GameException(std::string message, std::string description)
	: exception(message.c_str()),
	  _message(message),
	  _description(description)
{

}

std::string GameException::getMessage()
{
	return _message;
}

std::string GameException::getDescription()
{
	return _description;
}