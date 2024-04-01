#include "../../includes/Commands.hpp"

// send a detailed information about the requested user (/whois <nickname>)
void Commands::whois(int socket, const std::string &msg)
{
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;
	std::string clientNickname = *itToken;
	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		if (itClient->second.Nickname == clientNickname)
			break;
	}
	if (itClient == clients.end())
	{
		replyMsg = ERR_NOSUCHNICK(clientNickname, clients[socket].Nickname);
		srv->Send(socket, replyMsg);
		return;
	}
	replyMsg = RPL_WHOIS(clients[socket].Nickname, clientNickname, itClient->second.Username, itClient->second.Hostname, itClient->second.Realname);
	srv->Send(socket, replyMsg);
}