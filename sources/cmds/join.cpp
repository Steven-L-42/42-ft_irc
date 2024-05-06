#include "../../includes/Commands.hpp"

// Join help user to join a channel (/join <channel>)
void Commands::join(int socket, const std::string &msg)
{
	bool isNewChannel = false;

	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	std::string tok_Channel = *itToken;
	if (tok_Channel[0] != '#')
		tok_Channel = "#" + tok_Channel;
	std::string tok_Passsword = "";
	if (++itToken != strTokens.end())
		tok_Passsword = *itToken;

	if (channels.find(tok_Channel) == channels.end())
	{
		Channel newChannel;
		newChannel.Password = tok_Passsword;
		newChannel.has_password = true;
		newChannel.invite_only = false;
		newChannel.join_invite_only = false;
		newChannel.restricted_topic = true;
		newChannel.user_count = 1;
		newChannel.max_users = MAX_USERS;
		if (tok_Passsword == "")
			newChannel.has_password = false;
		newChannel.Modes = "+t";
		newChannel.Topic = "No Topic";
		channels[tok_Channel] = newChannel;
		isNewChannel = true;
	}
	else
	{
		if (clients[socket].channels[tok_Channel].isJoined == true)
		{
			replyMsg = ERR_USERONCHANNEL(clients[socket].Nickname, channelName);
			srv->Send(socket, replyMsg);
			return;
		}

		if (channels[tok_Channel].join_invite_only == false)
		{
			if (channels[tok_Channel].invite_only == true)
			{
				replyMsg = ERR_INVITEONLYCHAN(clients[socket].Nickname, tok_Channel);
				srv->Send(socket, replyMsg);
				return;
			}
		}

		if (channels[tok_Channel].user_count >= channels[tok_Channel].max_users)
		{
			replyMsg = ERR_CHANNELISFULL(clients[socket].Nickname, tok_Channel);
			srv->Send(socket, replyMsg);
			return;
		}

		std::string channel_Password_trimmed = Helper::trim_whitespace_jan(channels[tok_Channel].Password);
		std::string tok_Passsword_trimmed = Helper::trim_whitespace_jan(tok_Passsword);
		if (channel_Password_trimmed != tok_Passsword_trimmed && channels[tok_Channel].join_invite_only == false)
		{
			clients[socket].channels[tok_Channel].isJoined = false;
			replyMsg = ERR_PASSWDMISMATCH(clients[socket].Nickname);
			srv->Send(socket, replyMsg);
			return;
		}
		channels[tok_Channel].user_count++;
		channels[tok_Channel].join_invite_only = false;
	}

	// set REPLY MSG and send it back to clientSocket
	replyMsg = RPL_JOINMSG(clients[socket].Nickname, clients[socket].Username, clients[socket].Hostname, tok_Channel);

	// add channel to user
	clients[socket].channels[tok_Channel].isJoined = true;

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->second.channels[tok_Channel].isJoined == true)
			srv->Send(itClient->first, replyMsg);
	}
	// Besides first user also check if its the bot BITCHBOT
	if (isNewChannel)
	{
		clients[socket].channels[tok_Channel].isOp = true;
		replyMsg = RPL_UMODEIS(clients[socket].Hostname, tok_Channel, "+o", clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		if (channels[tok_Channel].Password != "")
			mode(socket, "MODE " + tok_Channel + " +k " + tok_Passsword + "\r\n");
	}
}