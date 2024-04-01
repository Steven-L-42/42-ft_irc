#include "../../includes/Commands.hpp"

// display or change a topic (only operator) (/topic or /topic <new Topic>)
void Commands::topic(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	channelName = *itToken;

	// Do nothing if # ist not in front of channelName (compared to official Server)
	if (channelName[0] != '#')
		return;

	// Send current channel topic to client
	if (strTokens.size() == 2)
	{
		// send Channel Topic
		replyMsg = RPL_CHTOPIC(clients[socket].Nickname, channelName, channels[channelName].Topic);
		srv->Send(socket, replyMsg);
		return;
	}
	// if more then 2 && client is Operator then change channel topic
	if (clients[socket].channels[channelName].isOp == true)
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
		replyMsg = RPL_SETCHTOPIC(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, channelName, message);
		srv->Send(socket, replyMsg);
	}
	else
	{
		// client is not an operator - Error
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channelName);
		srv->Send(socket, replyMsg);
	}
}
