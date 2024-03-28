#pragma once

#include <iostream>

//
class JOIN
{
public:
	static void join(int _clientSocket, const std::string &msg);
};

void JOIN::join(int _clientSocket, const std::string &msg)
{
	clients[_clientSocket].recvMsg = "";
		bool isNewChannel = false;
		std::string replyMsg;

		std::vector<std::string> strTokens = Helper::splitString(msg);
		std::vector<std::string>::iterator itToken = strTokens.begin();
		itToken++;
		std::string tok_Channel = *itToken;
		std::string tok_Passsword = "";
		if (++itToken != strTokens.end())
			tok_Passsword = *itToken;

		if (channels.find(tok_Channel) == channels.end())
		{
			Channel newChannel;
			newChannel.Password = tok_Passsword;
			newChannel.Modes = "";
			newChannel.Topic = "No Topic";
			channels[tok_Channel] = newChannel;
			isNewChannel = true;
		}
		else
		{
			if (channels[tok_Channel].Password != tok_Passsword)
			{
				replyMsg = ERR_PASSWDMISMATCH(clients[_clientSocket].Nickname);
				srvSend(_clientSocket, replyMsg);
				return (true);
			}
		}

		// set REPLY MSG and send it back to clientSocket
		replyMsg = RPL_JOINMSG(clients[_clientSocket].Nickname, clients[_clientSocket].Hostname, tok_Channel);

		// add channel to user
		clients[_clientSocket].channel[tok_Channel].isJoined = true;

		std::map<int, Client>::iterator it;
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second.channel[tok_Channel].isJoined == true)
				srvSend(it->first, replyMsg);
		}

		if (isNewChannel)
		{
			clients[_clientSocket].channel[tok_Channel].isOp = true;
			replyMsg = RPL_UMODEIS(clients[_clientSocket].Hostname, tok_Channel, "+o", clients[_clientSocket].Nickname);
			srvSend(_clientSocket, replyMsg);
		}
}
