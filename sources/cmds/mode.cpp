#include "../../includes/Commands.hpp"
#include "../../includes/Helper.hpp"
#include "../../includes/Channel.hpp"

bool Commands::k_password(std::string mode, std::string password)
{
	if (mode == "+k" || mode == "-k")
	{
		if (mode == "-k")
		{
			channels[channelName].has_password = false;
			channels[channelName].Password = "";
			return true;
		}
		else
		{
			channels[channelName].Password = password;
			channels[channelName].has_password = true;
			return true;
		}
	}
	return false;
}

// We need to implement a restriction to operators to use /topic command if +t is set.
bool Commands::t_topic(std::string mode)
{

	if (mode == "+t" || mode == "-t")
	{
		if (mode == "-t")
		{
			channels[channelName].restricted_topic = false;
			return true;
		}
		else
		{
			channels[channelName].restricted_topic = true;
			return true;
		}
	}
	return false;
}

bool Commands::i_invite(std::string mode)
{
	if (mode == "+i" || mode == "-i")
	{
		if (mode == "+i")
			channels[channelName].invite_only = true;
		else
			channels[channelName].invite_only = false;
		return true;
	}
	return false;
}

bool Commands::o_operator(int socket, std::string mode, std::string nickname)
{

	if (mode == "+o" || mode == "-o")
	{
		// looking for the user
		for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		{
			if (itClient->second.Nickname == nickname)
			{
				if (mode == "+o")
					itClient->second.channels[channelName].isOp = true;
				else
					itClient->second.channels[channelName].isOp = false;
				return true;
			}
		}
		if (itClient == clients.end())
		{
			replyMsg = ERR_NOSUCHNICK(nickname, clients[socket].Nickname);
			srv->Send(socket, replyMsg);
		}
	}
	return false;
}

bool Commands::l_userLimit(int socket, std::string mode, std::string param)
{

	if (mode == "+l" || mode == "-l")
	{
		if (mode == "+l")
		{
			if (param == "")
			{
				replyMsg = ERR_NEEDMOREPARAMS(clients[socket].Nickname);
				srv->Send(socket, replyMsg);
				return;
			}

			std::istringstream iss(param);
			size_t limit;

			if (!(iss >> limit))
				return std::cout << ("Ungültige Zeichenfolge: " + param) << std::endl, false;

			std::string remaining;
			if (iss >> remaining)
				return std::cout << ("Ungültige Zeichenfolge: " + param) << std::endl, false;

			if (limit > std::numeric_limits<size_t>::max())
				return std::cout << ("Zu große Zahl: " + param) << std::endl, false;

			channels[channelName].max_users = limit;
		}
		else
			channels[channelName].max_users = MAX_USERS;
		return true;
	}
	return false;
}
// MODE (Operator Command) - Change the channel’s mode:
// Done ! i: Set/remove Invite-only channel
// Done ! t: Set/remove the restrictions of the TOPIC command to channel operators
// Done ! k: Set/remove the channel key (password)
// Done ! o: Give/take channel operator privilege
// Done ! l: Set/remove the user limit to channel
void Commands::mode(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);

	if (strTokens.size() < 3)
	{
		replyMsg = ERR_NEEDMOREPARAMS(clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}
	itToken = strTokens.begin();
	itToken++;
	channelName = *itToken++;
	std::string mode = *itToken++;
	std::string param = "";

	if (strTokens.size() > 3)
	{
		while (itToken != strTokens.end())
		{
			param += *itToken;
			if (++itToken != strTokens.end())
				param += " ";
		}
	}

	if (channelName[0] != '#')
		channelName = "#" + channelName;

	// looking for the channel
	if (channels.find(channelName) == channels.end())
	{
		replyMsg = ERR_NOSUCHCHANNEL(channelName, clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}

	if (clients[socket].channels[channelName].isOp == false)
	{
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channelName);
		srv->Send(socket, replyMsg);
		return;
	}

	if (o_operator(socket, mode, param) == true || k_password(mode, param) == true || l_userLimit(socket, mode, param))
	{
		replyMsg = RPL_CHANNELMODEIS(clients[socket].Nickname, channelName, mode + (param != "" ? " " + param : ""));
		srv->Send(socket, replyMsg);
		return;
	}

	if (t_topic(mode) == true || i_invite(mode) == true)
	{
		replyMsg = RPL_CHANNELMODEIS(clients[socket].Nickname, channelName, mode);
		srv->Send(socket, replyMsg);
		return;
	}

	replyMsg = ERR_UNKNOWNMODE(clients[socket].Nickname, channelName, mode);
	srv->Send(socket, replyMsg);
	return;
}
