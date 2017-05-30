#pragma once

#include <stdexcept>
#include <string>

namespace osgGaming
{
	class GameException : public std::runtime_error
	{
	public:
		GameException(std::string message);
		GameException(std::string message, std::string description);

		std::string getMessage();
		std::string getDescription();

	private:
		std::string _message;
		std::string _description;
	};
}
