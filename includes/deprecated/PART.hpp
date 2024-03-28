#pragma once

#include <iostream>

//
class PART
{
public:
	static void part(int _clientSocket, const std::string &msg);
};

void PART::part(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string ChannelName = *itToken;
		// set REPLY MSG and send it back to clientSocket
		std::string replyMsg = RPL_PART(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, ChannelName);
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.channel[ChannelName].isJoined == true)
				srvSend(it->first, replyMsg);
		}
		// remove channel from user
		if (clients[_clientSocket].channel[ChannelName].isJoined == true)
			clients[_clientSocket].channel.erase(clients[_clientSocket].channel.find(ChannelName));
}
