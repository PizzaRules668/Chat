#include <string>
#include "Message.cpp"
#include "E:/Scripts/Chat/Chat/Third Party/easycpp/tupple.h"

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
