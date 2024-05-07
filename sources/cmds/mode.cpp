#include "../../includes/Commands.hpp"
#include "../../includes/Helper.hpp"
#include "../../includes/Channel.hpp"

bool Commands::k_password(int socket, std::string mode, std::string password)
{
	if (mode == "+k" || mode == "-k")
	{
		if (mode == "-k")
		{
			if (channels[channelName].Password != password)
			{
				had_Error = true;
				replyMsg = ERR_PASSWDMISMATCH(clients[socket].Nickname);
				srv->Send(socket, replyMsg);
				return true;
			}
			channels[channelName].has_password = false;
			channels[channelName].Password = "";
		}
		else
		{
			channels[channelName].Password = password;
			channels[channelName].has_password = true;
		}
		removeOrAdd_Mode(mode);
		return true;
	}
	return false;
}

bool Commands::t_topic(std::string mode)
{

	if (mode == "+t" || mode == "-t")
	{
		if (mode == "-t")
			channels[channelName].restricted_topic = false;
		else
			channels[channelName].restricted_topic = true;
		removeOrAdd_Mode(mode);
		return true;
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
		removeOrAdd_Mode(mode);
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
				removeOrAdd_Mode(mode);
				return true;
			}
		}
		if (itClient == clients.end())
		{
			had_Error = true;
			replyMsg = ERR_NOSUCHNICK(nickname, clients[socket].Nickname);
			srv->Send(socket, replyMsg);
			return true;
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
				had_Error = true;
				replyMsg = ERR_NEEDMOREPARAMS(clients[socket].Nickname);
				srv->Send(socket, replyMsg);
				return true;
			}

			std::istringstream iss(param);
			int limit;

			if (!(iss >> limit) || limit > std::numeric_limits<int>::max() || limit <= 0)
			{
				channels[channelName].user_limit = false;
				channels[channelName].max_users = std::numeric_limits<int>::max();
				removeOrAdd_Mode("-l");
				return true;
			}

			channels[channelName].max_users = limit;
			channels[channelName].user_limit = true;
		}
		else
		{
			channels[channelName].user_limit = false;
			channels[channelName].max_users = std::numeric_limits<int>::max();
		}
		removeOrAdd_Mode(mode);
		return true;
	}
	return false;
}

bool Commands::removeOrAdd_Mode(std::string mode)
{
	if (mode[0] == '+')
	{
		mode.erase(0, 1);
		std::size_t pos = channels[channelName].Modes.find(mode);
		if (pos == std::string::npos)
			channels[channelName].Modes += mode;
	}
	else if (mode[0] == '-')
	{
		mode.erase(0, 1);
		std::size_t pos = channels[channelName].Modes.find(mode);
		if (pos != std::string::npos)
			channels[channelName].Modes.erase(pos, pos + 1);
	}

	channels[channelName].Modes_Params = "";
	std::string::iterator itModes = channels[channelName].Modes.begin();
	for (; itModes != channels[channelName].Modes.end(); itModes++)
	{
		if (*itModes == 'l')
			channels[channelName].Modes_Params += Helper::itoa(channels[channelName].max_users) + " ";
		if (*itModes == 'k')
			channels[channelName].Modes_Params += channels[channelName].Password + " ";
	}

	return true;
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

	if (strTokens.size() < 2)
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
	had_Error = false;

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

	if (mode[0] != '+' && mode[0] != '-')
		mode = "+" + mode;

	// looking for the channel
	if (channels.find(channelName) == channels.end())
	{
		replyMsg = ERR_NOSUCHCHANNEL(channelName, clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}

	// send current set modes to client
	if (strTokens.size() == 2)
	{
		if (channels[channelName].Modes != "+")
		{
			replyMsg = RPL_CHANNELMODEIS(clients[socket].Nickname, channelName, channels[channelName].Modes + " " + channels[channelName].Modes_Params);
			srv->Send(socket, replyMsg);
		}
		return;
	}

	// check if client is op
	if (clients[socket].channels[channelName].isOp == false)
	{
		replyMsg = ERR_CHANOPRIVSNEEDED(clients[socket].Nickname, channelName);
		srv->Send(socket, replyMsg);
		return;
	}

	if (o_operator(socket, mode, param) == true || k_password(socket, mode, param) == true || l_userLimit(socket, mode, param))
	{
		if (had_Error == true)
			return;
		replyMsg = RPL_CHANNELMODEIS(clients[socket].Nickname, channelName, mode + (param != "" ? " " + param : ""));
		std::map<int, Client>::iterator itClientstest;
		for (itClientstest = clients.begin(); itClientstest != clients.end(); itClientstest++)
		{
			// std::cout << itClientstest->first << std::endl;
			if (itClientstest->second.channels.find(channelName) != itClientstest->second.channels.end())
			{
				// std::cout << "BITCH" << std::endl;
				srv->Send(itClientstest->first, replyMsg);
			}
		}

		// srv->Send(socket, replyMsg);
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
