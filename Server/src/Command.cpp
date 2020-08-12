#include <vector>
#include <string>
#include <variant>

#include "Message.cpp"

#pragma once

struct Command
{
	std::string command;
	Message message;
	std::vector<std::variant<std::string, bool, std::vector<std::string>>> args;

	void(*execute)(std::vector<std::variant<std::string, bool, std::vector<std::string>>> args);

	void checkForCommand()
	{
		if (message.content == command)
		{
			execute(args);
		}
	}
};
