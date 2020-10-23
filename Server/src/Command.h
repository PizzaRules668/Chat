#include <vector>
#include <string>
#include <variant>
#include <sstream>

#include "Message.cpp"

#pragma once

namespace Server
{
	struct Args
	{
		std::vector<std::string> usernames;
		std::vector<std::string> ipAddress;
		Messages message;
	};

	struct returntype
	{
		std::string usernames;
		std::string ipAddress;

		bool ran = false;
	};

	class Command
	{
	public:
		Args args;
		std::string command;
		Server::returntype results;
		returntype(*execute)(Args args);

		void checkForCommand();

		Command(std::string commandInput, returntype(*executeInput)(Args args));
		~Command();
	};
}