#include "../../includes/Commands.hpp"

// Sends and handles private and channel messages
void Commands::privmsg(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
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


	replyMsg = RPL_MESSAGE(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, NickOrChannel, message);


	// if i private mail to other
	if (NickOrChannel[0] != '#')
	{
		std::map<int, Client>::iterator itKickUser;
		// search for user in clients
		for (itKickUser = clients.begin(); itKickUser != clients.end(); itKickUser++)
		{
			if (itKickUser->second.Nickname == NickOrChannel)
				break;
		}
		srv->Send(itKickUser->first, replyMsg);
	}
	// if i private mail myself
	else if (clients[socket].Nickname == NickOrChannel)
		srv->Send(socket, replyMsg);
	else
	{
		// send message to all in channel, but not myself
		if (clients[socket].channels.find(NickOrChannel) == clients[socket].channels.end())
			return ;
		for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		{
			if (itClient->first != socket && itClient->second.channels.find(NickOrChannel) != itClient->second.channels.end())
				srv->Send(itClient->first, replyMsg);
		}
	}
}