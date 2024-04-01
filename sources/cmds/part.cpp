#include "../../includes/Commands.hpp"

// helps a user to leave a channel (/leave <channel> <with or without reason>)
void Commands::part(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	channelName = *itToken++;

	std::string reason = "";
	if (itToken != strTokens.end())
	{
		itToken->erase(0, 1);
		for (; itToken != strTokens.end();)
		{
			reason += *itToken;
			if (++itToken != strTokens.end())
				reason += " ";
		}
	}
	else
		reason = "none";

	// set REPLY MSG and send it back to clientSocket
	replyMsg = RPL_PART(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, channelName, reason);

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->second.channels[channelName].isJoined == true)
			srv->Send(itClient->first, replyMsg);
	}
	// remove channel from user
	if (clients[socket].channels[channelName].isJoined == true)
		clients[socket].channels.erase(clients[socket].channels.find(channelName));
}