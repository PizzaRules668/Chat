#include <iostream>
#include <string>
#include <WS2tcpip.h>

struct Message
{
	std::string content, commandText;

	bool command;

	void process()
	{
		command = false;

		if (content.rfind("/", 0) == 0)
		{
			command = true;
			commandText = content;

			commandText.erase(0);
		}
		if (content.rfind("/", 0) != 0)
		{
			command = false;
		}
		return;
	}
};