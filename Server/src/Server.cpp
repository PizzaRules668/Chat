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

std::vector<std::string> splitAt(std::string content, char split)
{
	std::vector<std::string> strings;

	std::string temp;

	for (char i : content)
	{
		if (i == split)
		{
			strings.push_back(temp);
			temp = "";
		}
		else
			temp += i;
	}
	strings.push_back(temp);

	return strings;
}

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

	std::vector<std::string> command = splitAt(content, ' ');
	std::vector<std::string> commandArgs = splitAt(command.at(1), ':');

	std::string username = commandArgs.at(0);

	returnValue.usernames = username;

	return returnValue;
}

Server::returntype kick(Server::Args args)
{
	Server::returntype returntype;

	std::vector<std::string> command = splitAt(args.message.content, ' ');

	for (int i = 0; i < args.usernames.size(); i++)
		if (args.usernames.at(i) == command.at(0))
		{
			returntype.pos = i;
			break;
		}

	return returntype;
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
	Server::Command KickCommand("kick", kick, true);
	Server::Command ConnectCommand("connect", onConnect);
	Server::Command ActiveUserCommand("active", getUsers);

	std::vector<std::string> oped = { "Colin" };
	std::vector<std::string> users;
	std::vector<std::string> ip;
	std::vector<std::string> messages;

	std::string notAdmin = "You are not an admin you can not run that command\n or that is not a valid command";
	std::string kicked = "You have been kicked by an admin";

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
					Server::returntype kickReturn = KickCommand.checkForCommand(args);
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

					else if (kickReturn.ran)
					{
						int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

						for (int x = 0; x < socketCount; x++)
						{
							SOCKET socket = copy.fd_array[x];

							std::cout << kickReturn.pos << std::endl;
							std::cout << ip.at(kickReturn.pos) << std::endl;
							std::cout << getIP(socket) << "==" << ip.at(kickReturn.pos) << std::endl;

							if (getIP(socket) == ip.at(kickReturn.pos))
							{
								send(socket, kicked.c_str(), kicked.size() + 1, 0);

								closesocket(socket);
								FD_CLR(socket, &master);
							}
						}
					}

					else if (quitReturn.ran)
					{
						send(sock, notAdmin.c_str(), notAdmin.size() + 1, 0);
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