#include "../../includes/Commands.hpp"

// quits a user from the channel at request (/quit <with or without reason>)
void Commands::quit(int socket, const std::string &msg)
{
	// substract whole message after 'QUIT '
	strTokens = Helper::splitString(msg);
	itToken = strTokens.begin();
	itToken++;

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
	// prepare reply and set connected to false
	replyMsg = RPL_QUIT(nickname, username, hostname, reason);
	clients[socket].Connected = false;

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
		srv->Send(itClient->first, replyMsg);
}