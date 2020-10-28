#include "Command.h"

Server::Args::Args(std::vector<std::string> usernamesInput, std::vector<std::string> ipAddressInput, std::vector<std::string> opedInput, Messages messageInput, std::string ranBy)
{
	usernames = usernamesInput;
	ipAddress = ipAddressInput;
	oped = opedInput;
	message = messageInput;
	ranByUser = ranBy;
}

Server::Command::Command(std::string command, returntype(*executeInput)(Args args), bool adminRequired)
{
	execute = executeInput;
	commandName = command;
	admin = adminRequired;
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
		if (admin)
		{
			for (std::string oped : args.oped)
				if (oped == args.ranByUser)
				{
					results = execute(args);

					results.ran = true;
				}
		}
		else if (!admin)
		{
			results = execute(args);

			results.ran = true;
		}
	}
	return results;
}
