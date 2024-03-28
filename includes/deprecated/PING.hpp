#pragma once

#include <iostream>

//
class PING
{
public:
	static void ping(int _clientSocket, const std::string &msg);
};

void PING::ping(int _clientSocket, const std::string &msg)
{
			clients[_clientSocket].recvMsg = "";
		std::string replyMsg = RPL_PONG(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname);
		srvSend(_clientSocket, replyMsg);
}
