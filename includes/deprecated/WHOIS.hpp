#pragma once

#include <iostream>

//
class WHOIS
{
public:
	static void whois(int _clientSocket, const std::string &msg);
};

void WHOIS::whois(int _clientSocket, const std::string &msg)
{
	std::string replyMsg;
		clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string clientNickname = *itToken;
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.Nickname == clientNickname)
				break;
		}
		if (it == clients.end())
			return (true);

		replyMsg = RPL_WHOIS(clients[_clientSocket].Nickname, clientNickname, it->second.Username, it->second.Hostname, it->second.Realname);
		srvSend(_clientSocket, replyMsg);
}
