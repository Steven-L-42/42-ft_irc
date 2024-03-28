#pragma once

#include <iostream>

//
class MODE
{
public:
	static void mode(int _clientSocket, const std::string &msg);
};

void MODE::mode(int _clientSocket, const std::string &msg)
{
	(void)msg;
		(void)cmd;
		// clients[_clientSocket].recvMsg = "";
		// // substract channel name without # and without \r\n
		// std::string substrCHANNEL = msg.substr(modeSize, msg.size() - modeSize - 2);
		// std::string replyMsg;

		// // send Channel modes
		// replyMsg = RPL_CHANNELMODEIS(clients[_clientSocket].Nickname, substrCHANNEL, " ");
		// srvSend(_clientSocket, replyMsg);

		clients[_clientSocket].recvMsg = "";
		std::string replyMsg = ": 421 " + clients[_clientSocket].Nickname + " MODE :Command is currently deactivated by Steven!" + CRLF;
		srvSend(_clientSocket, replyMsg);
		return (true);
}
