#include <string>
#include "Message.cpp"
#include "Command.cpp"
#include "tupple.h"

#pragma once

struct Command
{
	std::string command;
	Message message;
	easycpp::Tupple args;

	void(*execute)(easycpp::Tupple args1);

	void checkForCommand();
};
