#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma once

struct Message
{
	std::string content, username;

	bool command;

	void process()
	{
		command = false;

		if (content.rfind("/", 0) == 0)
		{
			command = true;
		}
		if (content.rfind("/", 0) != 0)
		{
			command = false;
		}
		return;
	}

	void sendProcessedMessage(SOCKET sock)
	{
		if (command)
		{
			send(sock, content.c_str(), content.size(), 0);
		}
		if (!command)
		{
			std::string message = username + ":" + content + "\n";

			int sendResult = send(sock, message.c_str(), message.size(), 0);

			return;
		}
	}
};