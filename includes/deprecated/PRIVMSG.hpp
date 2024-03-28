#pragma once

#include <iostream>

//
class PRIVMSG
{
public:
	static void privmsg(int _clientSocket, const std::string &msg);
};

void PRIVMSG::privmsg(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string NickOrChannel = *itToken;
		std::string message = "";
		itToken++;
		while (itToken != strTokens.end())
		{
			message += *itToken;
			if (++itToken != strTokens.end())
				message += " ";
		}
		std::string replyMsg = RPL_MESSAGE(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, NickOrChannel, message);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->first == _clientSocket && clients[_clientSocket].Nickname != NickOrChannel)
				continue;
			srvSend(it->first, replyMsg);
		}
}
