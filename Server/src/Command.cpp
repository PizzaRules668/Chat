#include <string>
#include "Message.cpp"
#include "tupple.h"

struct Command
{
	std::string command;
	Message message;
	easycpp::Tupple args;

	void(*execute)(easycpp::Tupple args1);

	void checkForCommand()
	{
		if (message.content == command)
		{
			execute(args);
		}
	}
};
