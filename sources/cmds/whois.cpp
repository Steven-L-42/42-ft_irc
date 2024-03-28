#include "../../includes/Commands.hpp"

void Commands::whois(int socket, const std::string &msg)
{
	clients[socket].recvMsg = "";
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
		return;

	replyMsg = RPL_WHOIS(clients[socket].Nickname, clientNickname, itClient->second.Username, itClient->second.Hostname, itClient->second.Realname);
	srv->Send(socket, replyMsg);
}