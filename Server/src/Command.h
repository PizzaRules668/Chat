#include <vector>
#include <string>
#include <variant>
#include <sstream>

#include "Message.cpp"

#pragma once

namespace Server
{
	class Args
	{
	public:
		std::vector<std::string> usernames;
		std::vector<std::string> ipAddress;
		std::vector<std::string> oped;

		Messages message;
		Args(std::vector<std::string> usernamesInput, std::vector<std::string> ipAddressInput, std::vector<std::string> opedInput, Messages messageInput);
	};

	struct returntype
	{
		std::string usernames;
		std::string ipAddress;

		bool ran;
	};

	class Command
	{
	private:
		std::string commandName;
		returntype(*execute)(Args args);

	public:
		returntype checkForCommand(Args args);

		Command(std::string command, returntype(*executeInput)(Args args));
		~Command();
	};
}