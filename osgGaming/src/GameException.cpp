#include <osgGaming/GameException.h>

#include <string>
#include <exception>

namespace osgGaming
{

    GameException::GameException(std::string message)
        : std::runtime_error(message.c_str())
        , _message(message)
    {

    }

    GameException::GameException(std::string message, std::string description)
        : std::runtime_error(message.c_str())
        , _message(message)
        , _description(description)
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

}
