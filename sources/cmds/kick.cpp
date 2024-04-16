#include "../../includes/Commands.hpp"

// kicks a user from current channel (/kick <user>) (only operator)
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
		std::map<int, Client>::iterator itKickUser;
		// search for user in clients
		for (itKickUser = clients.begin(); itKickUser != clients.end(); itKickUser++)
		{
			if (itKickUser->second.Nickname == kickedUser)
				break;
		}

		// if nickname does not exist, send error code.
		if (itKickUser == clients.end())
		{
			replyMsg = ERR_NOSUCHNICK(kickedUser, clients[socket].Nickname);
			srv->Send(socket, replyMsg);
			return;
		}
		else if (itKickUser->second.channels.find(channel) == itKickUser->second.channels.end())
		{
			replyMsg = ERR_NOTSAMECHANNEL(kickedUser, clients[socket].Nickname, channel);
			srv->Send(socket, replyMsg);
			return;
		}

		// parse the reason message
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

		// send the kicked message to all users on same channel
		for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		{
			if (itClient->second.channels.find(channel) != itClient->second.channels.end())
			{
				replyMsg = RPL_KICKUSER(nickname, username, hostname, channel, kickedUser, reason);
				srv->Send(itClient->first, replyMsg);
			}
		}
		clients[itKickUser->first].channels.erase(clients[itKickUser->first].channels.find(channel));
		if (channels[channel].user_count > 0)
			channels[channel].user_count--;
	}
	else
	{
		// client is not an operator - Error
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channel);
		srv->Send(socket, replyMsg);
	}
}
