#include "Command.h"

Server::Command::Command(std::string commandInput, returntype(*executeInput)(Args args))
{
	execute = executeInput;
	command = commandInput;
}

Server::Command::~Command(){}

void Server::Command::checkForCommand()
{
	std::string content = args.message.content;
	std::istringstream ss(content);

	std::vector<std::string> result;

	std::istringstream iss(content);
	for (std::string s; iss >> s;)
		result.push_back(s);

	if (result.at(0) == command)
	{
		results = execute(args);

		results.ran = true;
	}
}