#include "../../includes/Commands.hpp"

// kicks a user from current channel (/kick <user>)
void Commands::kick(int socket, const std::string &msg)
{
	// create tokens, split string on each space ' '.
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;

	std::string channel = *itToken++;
	std::string kickedUser = *itToken++;
	if (clients[socket].channels[channel].isOp == true)
	{
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

		std::string nickname = clients[socket].Nickname;
		std::string username = clients[socket].Username;
		std::string hostname = clients[socket].Hostname;

		for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		{
			if (itClient->second.channels.find(channel) != itClient->second.channels.end())
			{
				replyMsg = RPL_KICKUSER(nickname, username, hostname, channel, kickedUser, reason);
				srv->Send(itClient->first, replyMsg);
			}
		}
	}
	else
	{
		// client is not an operator - Error
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channel);
		srv->Send(socket, replyMsg);
	}
}
