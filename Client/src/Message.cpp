#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma once

struct Client
{
	std::string content, username;

	void process(SOCKET sock)
	{
		if (content.rfind("/", 0) == 0)
		{
			send(sock, content.c_str(), content.size(), 0);
		}

		if (content.rfind("/", 0) != 0)
		{
			std::string message = "\n" + username + ":" + content;

			int sendResult = send(sock, message.c_str(), message.size(), 0);

			return;
		}
	}
};