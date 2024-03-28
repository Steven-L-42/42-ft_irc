#include "../../includes/Commands.hpp"

void Commands::join(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	bool isNewChannel = false;

	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
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
			replyMsg = ERR_PASSWDMISMATCH(clients[socket].Nickname);
			srv->Send(socket, replyMsg);
			return;
		}
	}

	// set REPLY MSG and send it back to clientSocket
	replyMsg = RPL_JOINMSG(clients[socket].Nickname, clients[socket].Hostname, tok_Channel);

	// add channel to user
	clients[socket].channel[tok_Channel].isJoined = true;

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->second.channel[tok_Channel].isJoined == true)
			srv->Send(itClient->first, replyMsg);
	}

	if (isNewChannel)
	{
		clients[socket].channel[tok_Channel].isOp = true;
		replyMsg = RPL_UMODEIS(clients[socket].Hostname, tok_Channel, "+o", clients[socket].Nickname);
		srv->Send(socket, replyMsg);
	}
}