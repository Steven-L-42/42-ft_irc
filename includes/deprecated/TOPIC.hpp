#pragma once

#include <iostream>

//
class TOPIC
{
public:
	static void topic(int _clientSocket, const std::string &msg);
};

void TOPIC::topic(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string channelName = *itToken;

		// Do nothing if # ist not in front of channelname (compared to official Server)
		if (channelName[0] != '#')
			return (true);

		// Send current channel topic to client
		if (strTokens.size() == 2)
		{
			// send Channel Topic
			replyMsg = RPL_CHTOPIC(clients[_clientSocket].Nickname, channelName, channels[channelName].Topic);
			srvSend(_clientSocket, replyMsg);
			return (true);
		}
		// if more then 2 && client is Operator then change channel topic
		if (clients[_clientSocket].channel[channelName].isOp == true)
		{
			itToken++;
			std::string message = "";
			while (itToken != strTokens.end())
			{
				message += *itToken;
				if (++itToken != strTokens.end())
					message += " ";
			}
			message.erase(0, 1);
			channels[channelName].Topic = message;
			replyMsg = RPL_SETCHTOPIC(clients[_clientSocket].Nickname, clients[_clientSocket].Username, clients[_clientSocket].Hostname, channelName, message);
			srvSend(_clientSocket, replyMsg);
		}
		else
		{
			// client is not an operator - Error
			replyMsg = ERR_CHANOPRIVSNEEDED(clients[_clientSocket].Nickname, channelName);
			srvSend(_clientSocket, replyMsg);
		}
}
