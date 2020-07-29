#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

void sender(std::string username, SOCKET sock)
{
	while (true)
	{
		std::string userInput;

		std::getline(std::cin, userInput);

		if (userInput.size() > 0)
		{
			std::string message = username + ":" + userInput;

			int sendResult = send(sock, message.c_str(), message.size() + 1, 0);
		}
	}
}

void receiver(SOCKET sock, char* buf)
{
	ZeroMemory(buf, 4096);
	int bytesReceived = recv(sock, buf, 4096, 0);
	if (bytesReceived > 0)
	{
		std::cout << std::string(buf, 0, bytesReceived) << std::endl;
	}
}

int main()
{
	std::string username;
	std::cout << "What would you like to be called";
	std::cin >> username;
	std::string usernameform = username + ": Joined the chat";

	std::string ipAddress = "127.0.0.1";
	int port = 54000;

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
	send(sock, usernameform.c_str(), usernameform.size(), 0);

	std::thread sender(sender, username, sock);
	std::thread receiver(receiver, sock, buf);

	while (true);

	sender.join();
	receiver.join();

	closesocket(sock);
	WSACleanup();
}
