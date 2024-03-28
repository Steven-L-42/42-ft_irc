#pragma once

#include <iostream>

//
class QUIT
{
public:
	static void quit(int _clientSocket, const std::string &msg);
};

void QUIT::quit(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
		// substract whole message after 'QUIT :'
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string quitMessage = *itToken;

		// prepare reply and set connected to false
		std::string replyMsg = RPL_QUIT(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, quitMessage);
		clients[_clientSocket].Connected = false;
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
			srvSend(it->first, replyMsg);
}
