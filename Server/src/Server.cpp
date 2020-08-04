#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include "Message.h"
#include "Command.h"
#include "tupple.h"

#pragma comment (lib, "ws2_32.lib")

bool running = true;

void quit(easycpp::Tupple args)
{
	running = false;

	return;
}

int main()
{
	Message message;
	
	Command quit;
	quit.command = "quit";
	easycpp::Tupple quitargs = { running };
	quit.execute = [&](auto quitargs) {quit(quitargs); };

	std::vector<std::string> messages;

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


	std::cout << "Started";

	while (running)
	{

		fd_set copy = master;
		
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				SOCKET client = accept(listening, nullptr, nullptr);

				FD_SET(client, &master);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);

				message.process();

				

				if (bytesIn <= 0)
				{
					closesocket(sock);
					FD_CLR(sock, &master);
				}

				else
				{
					if (true)
					{

					}

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							std::ostringstream ss;
							ss << buf;
							std::string strOut = ss.str();
							messages.push_back(strOut);

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

	system("pause");
}