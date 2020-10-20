#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma once

namespace Message
{
	struct Messages
	{
		std::string content;
		bool isCommand;

		void process()
		{
			if (content.rfind("/", 0) == 0)
			{
				content.replace(0, 1, "");
				isCommand = true;
			}
			else
				isCommand = false;
			return;
		}
	};
}