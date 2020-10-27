#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma once

namespace Server
{
	struct Messages
	{
		std::string content;
		bool command;

		void process()
		{
			if (content.rfind("/", 0) == 0)
			{
				content.replace(0, 1, "");
				command = true;
			}
			else
				command = false;
			return;
		}
	};
}