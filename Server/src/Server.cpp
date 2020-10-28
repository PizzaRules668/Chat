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
	Server::returntype returnValue;
	std::cout << "Stopping" << std::endl;

	exit(0);
	return returnValue;
}

Server::returntype getUsers(Server::Args args)
{
	Server::returntype returnValue;
	std::string users;

	for (std::string i : args.usernames)
		users = i + "\n";

	returnValue.usernames = users;

	return returnValue;
}

Server::returntype onConnect(Server::Args args)
{
	Server::returntype returnValue;
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
	returnValue.usernames = username;

	return returnValue;
}

std::string recvToString(char* buf)
{
	std::ostringstream ss;
	ss << buf;
	std::string strOut = ss.str();

	return strOut;
}

std::string getIP(SOCKET sock)
{
	sockaddr_in addr;
	int addrSize = sizeof(addr);
	char addrIP[INET_ADDRSTRLEN];
	char addrPORT[INET_ADDRSTRLEN];
	std::string addrIPString;
	std::string addrPORTString;

	getpeername(sock, (sockaddr*)&addr, &addrSize);
	inet_ntop(AF_INET, &(addr.sin_addr), addrIP, INET_ADDRSTRLEN);

	for (char i : addrIP)
		if (addrIP == " ")
		{
			std::cout << addrIPString;
			break;
		}
		else
			addrIPString + i;

	return addrIPString;
}

std::string getUser(SOCKET sock, std::vector<std::string> usernames, std::vector<std::string> ips)
{
	std::string ip = getIP(sock);
	std::string user;

	for (int i = 0; i < usernames.size(); i++)
		if (ips.at(i) == ip)
			user = usernames.at(i);

	return user;
}

int main()
{
	Server::Messages message;
	Server::Command QuitCommand("quit", quit, true);
	Server::Command ConnectCommand("connect", onConnect);
	Server::Command ActiveUserCommand("active", getUsers);

	std::vector<std::string> oped = { "Pizzarules668" };
	std::vector<std::string> users;
	std::vector<std::string> ip;
	std::vector<std::string> messages;

	std::string noAdmin = "You are not an admin you can not run that command";

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
					std::string addrIP = getIP(client);

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

				Server::Args args(users, ip, oped, message, getUser(sock, users, ip));
				std::cout << getUser(sock, users, ip) << std::endl;

				std::cout << message.content << std::endl;

				if (message.command)
				{
					Server::returntype quitReturn = QuitCommand.checkForCommand(args);
					Server::returntype connectReturn = ConnectCommand.checkForCommand(args);
					Server::returntype activeReturn = ActiveUserCommand.checkForCommand(args);

					if (connectReturn.ran)
					{
						users.push_back(connectReturn.usernames);
						std::cout << connectReturn.usernames << std::endl;
					}

					else if (activeReturn.ran) 
					{
						for (std::string line : users)
							send(sock, line.c_str(), line.size() + 1, 0);
					}

					else if (!quitReturn.ran)
					{
						send(sock, noAdmin.c_str(), noAdmin.size() + 1, 0);
					}
				}

				if (bytesIn <= 0)
				{
					std::string addrIP = getIP(sock);

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
							if (message.command == false)
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