#include "Command.h"

Server::Args::Args(std::vector<std::string> usernamesInput, std::vector<std::string> ipAddressInput, std::vector<std::string> opedInput, Messages messageInput)
{
	usernames = usernamesInput;
	ipAddress = ipAddressInput;
	oped = opedInput;
	message = messageInput;
}

Server::Command::Command(std::string command, returntype(*executeInput)(Args args))
{
	execute = executeInput;
	commandName = command;
}

Server::Command::~Command() {}

Server::returntype Server::Command::checkForCommand(Args args)
{
	Server::returntype results;
	std::string content = args.message.content;
	std::istringstream ss(content);

	std::vector<std::string> enteredCommand;

	std::istringstream iss(content);
	for (std::string s; iss >> s;)
		enteredCommand.push_back(s);

	if (enteredCommand.at(0) == commandName)
	{
		results = execute(args);

		results.ran = true;
	}
	return results;
}
