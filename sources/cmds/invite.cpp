#include "../../includes/Commands.hpp"

// invites a user to channel (/invite <user>)
void Commands::invite(int socket, const std::string &msg)
{
	// create tokens, split string on each space ' '.
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;

	// std::string nickname = clients[socket].Nickname;
	// std::string username = clients[socket].Username;
	// std::string hostname = clients[socket].Hostname;

	if (strTokens.size() != 3)
	{
		replyMsg = ERR_NEEDMOREPARAMS(clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}

	std::string nickname = *itToken++;
	std::string channel = *itToken;
	if (channel[0] != '#')
		channel = "#" + channel;

	// looking for the channel
	if (channels.find(channel) == channels.end())
	{
		replyMsg = ERR_NOSUCHCHANNEL(channel, clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}

	// if client is operator of channel
	if (clients[socket].channels[channel].isOp == true)
	{
		// looking for the user
		for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		{
			if (itClient->second.Nickname == nickname)
			{
				std::string joinMsg = "JOIN " + channel;
				channels[channelName].join_invite_only = true;
				join(itClient->first, joinMsg);
				return;
			}
		}
		// if user not found
		replyMsg = ERR_NOSUCHNICK(nickname, clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}
	else
	{
		// client is not an operator - Error
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channel);
		srv->Send(socket, replyMsg);
	}
}
