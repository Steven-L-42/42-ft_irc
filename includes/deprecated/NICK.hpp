#pragma once

#include <iostream>

//
class NICK
{
public:
	static void nick(int _clientSocket, const std::string &msg);
};

void NICK::nick(int _clientSocket, const std::string &msg)
{
			clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string clientNickname = *itToken;
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_NICKCHANGE(clients[_clientSocket].Nickname, clientNickname);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
			srvSend(it->first, replyMsg);
		// overwrite old nickname
		clients[_clientSocket].Nickname = clientNickname;
}
