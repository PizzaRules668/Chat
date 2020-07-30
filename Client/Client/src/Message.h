#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma once

struct Message
{
	std::string content, username;

	void sendMessage(SOCKET sock)
	{
		std::string Message = username + ":" + content + "\n";

		int sendResult = send(sock, Message.c_str(), Message.size() + 1, 0);

		return;
	}
};