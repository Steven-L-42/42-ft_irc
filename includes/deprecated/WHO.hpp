#pragma once

#include <iostream>

//
class WHO
{
public:
	static void who(int _clientSocket, const std::string &msg);
};

void WHO::who(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string channelName = *itToken;

		// send Channel Topic
		replyMsg = RPL_CHTOPIC(clients[_clientSocket].Nickname, channelName, channels[channelName].Topic);
		srvSend(_clientSocket, replyMsg);
		// create userlist based on stored information of clients
		std::string userList = "";
		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end();)
		{
			if (it->second.channel[channelName].isJoined == true)
			{
				userList += (it->second.channel[channelName].isOp ? "@" : "") + it->second.Nickname;
				it++;
				if (it != clients.end())
					userList += " ";
			}
			else
				it++;
		}

		// send all channel user nicknames to client
		replyMsg = RPL_USERLIST(clients[_clientSocket].Nickname, channelName, userList);
		srvSend(_clientSocket, replyMsg);
		// send a end of userlist for client to know that this list is ending
		replyMsg = RPL_ENDOFUSERLIST(clients[_clientSocket].Nickname, channelName);
		srvSend(_clientSocket, replyMsg);
}
