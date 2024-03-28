#include "../../includes/Commands.hpp"

void Commands::part(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	channelName = *itToken;
	// set REPLY MSG and send it back to clientSocket
	replyMsg = RPL_PART(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, channelName);

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->second.channel[channelName].isJoined == true)
			srv->Send(itClient->first, replyMsg);
	}
	// remove channel from user
	if (clients[socket].channel[channelName].isJoined == true)
		clients[socket].channel.erase(clients[socket].channel.find(channelName));
}