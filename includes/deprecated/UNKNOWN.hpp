#pragma once

#include <iostream>

//
class UNKNOWN
{
public:
	static void unknown(int _clientSocket, const std::string &msg);
};

void UNKNOWN::unknown(int _clientSocket, const std::string &msg)
{
		clients[_clientSocket].recvMsg = "";
	std::string replyMsg = ERR_UNKNOWNCOMMAND(clients[_clientSocket].Nickname, msg);
	srvSend(_clientSocket, replyMsg);
}
