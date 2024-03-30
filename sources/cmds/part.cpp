#include "../../includes/Commands.hpp"

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
		if (itClient->second.channel[channelName].isJoined == true)
			srv->Send(itClient->first, replyMsg);
	}
	// remove channel from user
	if (clients[socket].channel[channelName].isJoined == true)
		clients[socket].channel.erase(clients[socket].channel.find(channelName));
}