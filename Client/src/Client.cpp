#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include <fstream>

#include "Message.cpp"

#pragma comment(lib, "ws2_32.lib")

void sender(SOCKET sock, Message message)
{
	std::string userInput;

	while (true)
	{
		std::cout << ">";

		std::getline(std::cin, userInput);

		if (userInput.size() > 0)
		{
			message.content = userInput;

			message.process(sock);
		}
	}
}

void receiver(SOCKET sock, char* buf)
{
	while (true)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		if (bytesReceived > 0)
		{
			std::cout << std::string(buf, 0, bytesReceived) << std::endl;
		}
	}
}

int main()
{
	Message message;

	std::string username;

	std::fstream config;
	config.open("config.txt", std::ios::out | std::ios::in);

	std::string ipAddress = "127.0.0.1";
	int port = 54000;

	if (config.is_open())
	{
		std::getline(config, ipAddress);
		if (ipAddress.size() == 0)
		{
			std::cout << "Please Put IP Address In config.txt" << std::endl;
			return 0;
		}
	}

	std::cout << "What would you like to be called ";
	std::cin >> username;

	message.username = username;

	std::string onConnect = "/connect " + username + ":";

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return 0;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}

	char buf[4096];
	std::string userInput;
	send(sock, onConnect.c_str(), onConnect.size(), 0);

	std::thread sender(sender, sock, message);
	std::thread receiver(receiver, sock, buf);

	while (true);

	sender.join();
	receiver.join();

	closesocket(sock);
	WSACleanup();
}
