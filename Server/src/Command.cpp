#include <vector>
#include <string>
#include <variant>

#include "Message.cpp"

#pragma once

namespace Message
{
	struct Command
	{
		std::string command;
		Message::Messages message;
		std::vector<std::variant<int, std::string, bool, Message::Messages>> args;

		void(*execute)(std::vector<std::variant<int, std::string, bool, Message::Messages>> args);

		void checkForCommand()
		{
			if (message.content == command)
			{
				std::cout << "Running function" << std::endl;

				execute(args);
			}
		}
	};
}