#pragma once

#include <iostream>

//
class KICK
{
public:
	static void kick(int _clientSocket, const std::string &msg);
};

void KICK::kick(int _clientSocket, const std::string &msg)
{
	std::cout << "KICK\n";
}
