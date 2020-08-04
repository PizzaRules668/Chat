#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include"Message.cpp"

#pragma once

struct Message
{
	std::string content, commandText;

	bool command;

	void process();
};