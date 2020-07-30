#include <string>
#include "Message.cpp"

class Command
{
public:
	std::string command;
	Message message;
	void(*execute)();

	void checkForCommand()
	{
		if (message.content == command)
		{
			execute();
		}
	}
};
