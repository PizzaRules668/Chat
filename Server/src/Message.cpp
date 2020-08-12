#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma once

struct Message
{
	std::string content;

	void process()
	{
		if (content.rfind("/", 0) == 0)
		{
			content.replace(0, 1, "");
		}
		return;
	}
};