#include "../../includes/Commands.hpp"

// send a list with all current active users to client
void Commands::who(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	channelName = *itToken;

	// send Channel Topic
	replyMsg = RPL_CHTOPIC(clients[socket].Nickname, channelName, channels[channelName].Topic);
	srv->Send(socket, replyMsg);
	// create userlist based on stored information of clients
	std::string userList = "";
	for (itClient = clients.begin(); itClient != clients.end();)
	{
		if (itClient->second.channels[channelName].isJoined == true)
		{
			userList += (itClient->second.channels[channelName].isOp ? "@" : "") + itClient->second.Nickname;
			itClient++;
			if (itClient != clients.end())
				userList += " ";
		}
		else
			itClient++;
	}

	// send all channel user nicknames to client
	replyMsg = RPL_USERLIST(clients[socket].Nickname, channelName, userList);
	srv->Send(socket, replyMsg);
	// send a end of userlist for client to know that this list is ending
	replyMsg = RPL_ENDOFUSERLIST(clients[socket].Nickname, channelName);
	srv->Send(socket, replyMsg);
}