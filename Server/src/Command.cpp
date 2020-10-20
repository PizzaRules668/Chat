#include <vector>
#include <string>
#include <variant>
#include <sstream>

#include "Message.cpp"

#pragma once

namespace Message
{
	struct Args
	{
		std::string string;
		Message::Messages message;
		std::vector<std::string> usernames;
		std::vector<std::string> ipAddress;
		std::string command;
	};

	struct returntype
	{
		std::string usernames;
		std::string ipAddress;

		bool ran = false;
	};

	struct Command
	{
		Args args;

		Message::returntype results;

		returntype(*execute)(Args args);

		void checkForCommand()
		{
			std::string content = args.message.content;
			std::istringstream ss(content);

			std::vector<std::string> result;

			std::istringstream iss(content);
			for (std::string s; iss >> s; )
				result.push_back(s);

			if (result.at(0) == args.command)
			{
				results = execute(args);

				results.ran = true;
			}
		}
	};
}