#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <variant>
#include <WinSock2.h>

#include "Message.cpp"
#include "Command.h"

#pragma comment (lib, "ws2_32.lib")

Server::returntype quit(Server::Args args)
{
	Server::returntype returnvalue;
	std::cout << "Stopping" << std::endl;

	exit(0);
	return returnvalue;
}

Server::returntype getUsers(Server::Args args)
{
	Server::returntype returnvalue;
	std::string users;

	for (std::string i : args.usernames)
		users = i + "\n";

	returnvalue.usernames = users;

	return returnvalue;
}

Server::returntype onConnect(Server::Args args)
{
	Server::returntype returnvalue;
	std::string content = args.message.content;
	std::string splitAt = ":";
	std::string users;
	std::string ipAddress;
	std::istringstream ss(content);
	std::string username;
	std::string ip;
	std::string word;

	do {
		ss >> word;

		size_t pos = 0;

		while ((pos = word.find(splitAt)) != std::string::npos) {
			username = word.substr(0, pos);
			word.erase(0, pos + splitAt.length());
		}

	} while (ss);
	returnvalue.usernames = username;

	return returnvalue;
}

std::string recvToString(char* buf)
{
	std::ostringstream ss;
	ss << buf;
	std::string strOut = ss.str();

	return strOut;
}

std::string ipFromSock(SOCKET sock)
{
	sockaddr_in addr;
	int addrSize = sizeof(addr);
	char addrIP[INET_ADDRSTRLEN];
	std::string addrIPString;

	getpeername(sock, (sockaddr*)&addr, &addrSize);
	inet_ntop(AF_INET, &(addr.sin_addr), addrIP, INET_ADDRSTRLEN);

	for (char i : addrIP)
		if (addrIP == " ")
			break;
		else
			addrIPString += i;

	return addrIPString;
}

int main()
{
	Server::Messages message;
	Server::Command QuitCommand("quit", quit);
	Server::Command ConnectCommand("connect", onConnect);
	Server::Command ActiveUserCommand("active", getUsers);

	std::vector<std::string> users;
	std::vector<std::string> ip;
	std::vector<std::string> messages;

	ConnectCommand.args.usernames = users;
	ConnectCommand.args.ipAddress = ip;

	ActiveUserCommand.args.usernames = users;

	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
		return 0;
	}

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	std::cout << "Started" << std::endl;

	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				SOCKET client = accept(listening, nullptr, nullptr);

				if (client != INVALID_SOCKET)
				{
					std::string addrIP = ipFromSock(client);

					ip.push_back(addrIP);
				}
				FD_SET(client, &master);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);

				message.content = recvToString(buf);
				message.process();

				std::cout << message.content << std::endl;

				if (message.isCommand)
				{
					QuitCommand.args.message = message;
					QuitCommand.checkForCommand();

					ConnectCommand.args.message = message;
					ConnectCommand.checkForCommand();

					if (ConnectCommand.results.ran)
					{
						users.push_back(ConnectCommand.results.usernames);
						std::cout << ConnectCommand.results.usernames << std::endl;

						ConnectCommand.results.usernames = "";
					}

					ActiveUserCommand.args.message = message;
					ActiveUserCommand.args.usernames = users;
					ActiveUserCommand.checkForCommand();

					if (ActiveUserCommand.results.ran)
					{
						for (std::string line : users)
							send(sock, line.c_str(), line.size() + 1, 0);

						ActiveUserCommand.results.ran = false;
					}
				}

				if (bytesIn <= 0)
				{
					std::string addrIP = ipFromSock(sock);

					for (int i = 0; i < ip.size(); i++)
					{
						if (ip.at(i) == addrIP)
						{
							ip.erase(ip.begin(), ip.begin() + i);
							users.erase(users.begin(), users.begin() + i);
						}
					}

					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							std::string strOut = recvToString(buf);
							messages.push_back(strOut);
							if (message.isCommand == false)
								send(outSock, strOut.c_str(), strOut.size(), 0);
						}
					}
				}
			}
		}
	}

	FD_CLR(listening, &master);
	closesocket(listening);

	std::string msg = "Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		SOCKET sock = master.fd_array[0];

		send(sock, msg.c_str(), msg.size() + 1, 0);

		FD_CLR(sock, &master);
		closesocket(sock);
	}

	std::ofstream output_file("log.txt");
	std::ostream_iterator<std::string> output_iterator(output_file, "\n");
	std::copy(messages.begin(), messages.end(), output_iterator);

	WSACleanup();

	return 0;
}